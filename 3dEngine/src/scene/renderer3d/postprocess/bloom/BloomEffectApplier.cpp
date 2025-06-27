#include <cstring>

#include <scene/renderer3d/VisualConfig.h>
#include <scene/renderer3d/postprocess/bloom/BloomEffectApplier.h>
#include <graphics/render/shader/ShaderBuilder.h>
#include <graphics/render/GenericRendererBuilder.h>
#include <graphics/render/GenericComputeBuilder.h>

namespace urchin {

    BloomEffectApplier::BloomEffectApplier(const Config& config, bool isTestMode) :
            config(config),
            isTestMode(isTestMode),
            sceneWidth(0),
            sceneHeight(0),
            preFilterTweak({}) {
        std::memset((void *)&preFilterTweak, 0, sizeof(preFilterTweak));

        checkConfig();

        float filterSoftCurve = ConfigService::instance().getFloatValue("bloom.filterSoftCurve");
        float threshold = ConfigService::instance().getFloatValue("bloom.filterThreshold");
        preFilterTweak.softCurveParams.X = threshold - filterSoftCurve;
        preFilterTweak.softCurveParams.Y = 2.0f * filterSoftCurve;
        preFilterTweak.softCurveParams.Z = 0.25f / std::max(filterSoftCurve, 0.0001f);
        preFilterTweak.threshold = threshold;
    }

    BloomEffectApplier::~BloomEffectApplier() {
        clearRenderers();
    }

    void BloomEffectApplier::refreshInputTexture(const std::shared_ptr<Texture>& inputHdrTexture) {
        if (inputHdrTexture.get() != this->inputHdrTexture.get()) {
            bool sizeUpdated = this->sceneWidth != inputHdrTexture->getWidth() || this->sceneHeight != inputHdrTexture->getHeight();
            bool firstTexture = inputHdrTexture.get() == nullptr;

            this->inputHdrTexture = inputHdrTexture;
            this->sceneWidth = inputHdrTexture->getWidth();
            this->sceneHeight = inputHdrTexture->getHeight();

            if (firstTexture || sizeUpdated) {
                refreshRenderers();
            } else {
                preFilterCompute->updateUniformTextureReader(PF_INPUT_TEX_UNIFORM_BINDING, TextureReader::build(this->inputHdrTexture, TextureParam::buildLinear()));
                combineRenderer->updateUniformTextureReader(CR_HDR_TEX_UNIFORM_BINDING, TextureReader::build(this->inputHdrTexture, TextureParam::buildLinear()));
            }
        }
    }

    const std::shared_ptr<Texture>& BloomEffectApplier::getOutputTexture() const {
        return bloomCombineTexture;
    }

    void BloomEffectApplier::refreshRenderers() {
        //clear existing
        clearRenderers();

        //create bloom step textures
        unsigned int textureWidth = sceneWidth / 2;
        unsigned int textureHeight = sceneHeight / 2;
        bloomStepTextures.clear();
        for (unsigned int i = 0; i < config.maxIterations; ++i) {
            std::string bloomTextureName = "bloom " + std::to_string(textureWidth) + "x" + std::to_string(textureHeight);
            bloomStepTextures.push_back(Texture::build(bloomTextureName, textureWidth, textureHeight, TextureFormat::B10G11R11_FLOAT));

            textureWidth = textureWidth / 2;
            textureHeight = textureHeight / 2;
            if (textureWidth <= 8 || textureHeight <= 8) {
                break;
            }
        }

        //fullscreen vertex/texture coordinate
        std::vector vertexCoord = {
                Point2(-1.0f, -1.0f), Point2(1.0f, -1.0f), Point2(1.0f, 1.0f),
                Point2(-1.0f, -1.0f), Point2(1.0f, 1.0f), Point2(-1.0f, 1.0f)
        };
        std::vector textureCoord = {
                Point2(0.0f, 0.0f), Point2(1.0f, 0.0f), Point2(1.0f, 1.0f),
                Point2(0.0f, 0.0f), Point2(1.0f, 1.0f), Point2(0.0f, 1.0f)
        };

        //shader constants
        BloomShadersConst bloomShadersConst = {};
        bloomShadersConst.qualityTextureFetch = (uint32_t)config.textureFetchQuality;
        bloomShadersConst.upSampleScale = UP_SAMPLE_SCALE;
        std::vector downSampleVarSize = {sizeof(bloomShadersConst.qualityTextureFetch)};
        std::vector upSampleVarSize = {sizeof(bloomShadersConst.qualityTextureFetch), sizeof(bloomShadersConst.upSampleScale)};

        //pre-filter
        preFilterShader = ShaderBuilder::createComputeShader("bloomPreFilter.comp.spv", std::make_unique<ShaderConstants>(downSampleVarSize, &bloomShadersConst), isTestMode);
        preFilterRenderTarget = std::make_unique<OffscreenRender>("bloom - pre filter", isTestMode, RenderTarget::NO_DEPTH_ATTACHMENT);
        static_cast<OffscreenRender*>(preFilterRenderTarget.get())->addOutputTexture(bloomStepTextures[0], LoadType::NO_LOAD, std::nullopt, OutputUsage::COMPUTE);
        preFilterRenderTarget->initialize();

        Point2 texelSize(1.0f / (float)inputHdrTexture->getWidth(), 1.0f / (float)inputHdrTexture->getHeight());
        preFilterCompute = GenericComputeBuilder::create("bloom - pre filter", *preFilterRenderTarget, *preFilterShader, Vector2(8, 8))
                ->addUniformData(PF_TWEAK_UNIFORM_BINDING, sizeof(preFilterTweak), &preFilterTweak)
                ->addUniformData(PF_TEXEL_SIZE_UNIFORM_BINDING, sizeof(texelSize), &texelSize)
                ->addUniformTextureReader(PF_INPUT_TEX_UNIFORM_BINDING, TextureReader::build(inputHdrTexture, TextureParam::buildLinear()))
                ->addUniformTextureOutput(PF_OUTPUT_TEX_UNIFORM_BINDING, bloomStepTextures[0])
                ->build();

        //down sample
        downSampleShader = ShaderBuilder::createComputeShader("bloomDownSample.comp.spv", std::make_unique<ShaderConstants>(downSampleVarSize, &bloomShadersConst), isTestMode);
        for (std::size_t outTexIndex = 1, i = 0; outTexIndex < bloomStepTextures.size(); ++outTexIndex, ++i) {
            std::size_t srcTexIndex = outTexIndex - 1;

            auto downSampleRenderTarget = std::make_unique<OffscreenRender>("bloom - down sample " + std::to_string(i), isTestMode, RenderTarget::NO_DEPTH_ATTACHMENT);
            downSampleRenderTarget->addOutputTexture(bloomStepTextures[outTexIndex], LoadType::NO_LOAD, std::nullopt, OutputUsage::COMPUTE);
            downSampleRenderTarget->initialize();

            texelSize = Point2(1.0f / (float)bloomStepTextures[srcTexIndex]->getWidth(), 1.0f / (float)bloomStepTextures[srcTexIndex]->getHeight());
            downSampleComputes.push_back(GenericComputeBuilder::create("bloom - down sample " + std::to_string(i), *downSampleRenderTarget, *downSampleShader, Vector2(8, 8))
                    ->addUniformData(DS_TEXEL_SIZE_UNIFORM_BINDING, sizeof(texelSize), &texelSize)
                    ->addUniformTextureReader(DS_INPUT_TEX_UNIFORM_BINDING, TextureReader::build(bloomStepTextures[srcTexIndex], TextureParam::buildLinear()))
                    ->addUniformTextureOutput(DS_OUTPUT_TEX_UNIFORM_BINDING, bloomStepTextures[outTexIndex])
                    ->build());
            downSampleRenderTargets.push_back(std::move(downSampleRenderTarget));
        }

        //up sample
        upSampleShader = ShaderBuilder::createShader("bloomUpSample.vert.spv", "bloomUpSample.frag.spv", std::make_unique<ShaderConstants>(upSampleVarSize, &bloomShadersConst), isTestMode);
        for (std::size_t srcTexIndex = bloomStepTextures.size() - 1, i = 0; srcTexIndex > 0; --srcTexIndex, ++i) {
            std::size_t outTexIndex = srcTexIndex - 1;

            auto upSampleRenderTarget = std::make_unique<OffscreenRender>("bloom - up sample " + std::to_string(i), isTestMode, RenderTarget::NO_DEPTH_ATTACHMENT);
            upSampleRenderTarget->addOutputTexture(bloomStepTextures[outTexIndex], LoadType::LOAD_CONTENT);
            upSampleRenderTarget->initialize();

            texelSize = Point2(1.0f / (float)bloomStepTextures[srcTexIndex]->getWidth(), 1.0f / (float)bloomStepTextures[srcTexIndex]->getHeight());
            upSampleRenderers.push_back(GenericRendererBuilder::create("bloom - up sample " + std::to_string(i), *upSampleRenderTarget, *upSampleShader, ShapeType::TRIANGLE)
                    ->addData(vertexCoord)
                    ->addData(textureCoord)
                    ->addUniformData(US_TEXEL_SIZE_UNIFORM_BINDING, sizeof(texelSize), &texelSize)
                    ->addUniformTextureReader(US_INPUT_TEX_UNIFORM_BINDING, TextureReader::build(bloomStepTextures[srcTexIndex], TextureParam::buildLinear()))
                    ->enableTransparency({BlendFunction::build(BlendFactor::ONE, BlendFactor::ONE, BlendFactor::ONE, BlendFactor::ONE)})
                    ->build());
            upSampleRenderTargets.push_back(std::move(upSampleRenderTarget));
        }

        //combine
        bloomCombineTexture = Texture::build("bloom - combine", sceneWidth, sceneHeight, VisualConfig::SCENE_TEXTURE_FORMAT);
        combineRenderTarget = std::make_unique<OffscreenRender>("bloom - combine", isTestMode, RenderTarget::NO_DEPTH_ATTACHMENT);
        combineRenderTarget->addOutputTexture(bloomCombineTexture);
        combineRenderTarget->initialize();

        combineShader = ShaderBuilder::createShader("bloomCombine.vert.spv", "bloomCombine.frag.spv", std::make_unique<ShaderConstants>(upSampleVarSize, &bloomShadersConst), isTestMode);
        texelSize = Point2(1.0f / (float)bloomStepTextures[0]->getWidth(), 1.0f / (float)bloomStepTextures[0]->getHeight());
        combineRenderer = GenericRendererBuilder::create("bloom - combine", *combineRenderTarget, *combineShader, ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformData(CR_TEXEL_SIZE_UNIFORM_BINDING, sizeof(texelSize), &texelSize)
                ->addUniformTextureReader(CR_BLOOM_STEP_TEX_UNIFORM_BINDING, TextureReader::build(bloomStepTextures[0], TextureParam::buildLinear()))
                ->addUniformTextureReader(CR_HDR_TEX_UNIFORM_BINDING, TextureReader::build(inputHdrTexture, TextureParam::buildLinear()))
                ->build();
    }

    void BloomEffectApplier::clearRenderers() {
        //combine
        combineRenderer.reset();
        if (combineRenderTarget) {
            combineRenderTarget->cleanup();
        }

        //up sample
        upSampleRenderers.clear();
        for (const auto& upSampleRenderTarget : upSampleRenderTargets) {
            upSampleRenderTarget->cleanup();
        }
        upSampleRenderTargets.clear();

        //down sample
        downSampleComputes.clear();
        for (const auto& downSampleRenderTarget : downSampleRenderTargets) {
            downSampleRenderTarget->cleanup();
        }
        downSampleRenderTargets.clear();

        //pre filter
        preFilterCompute.reset();
        if (preFilterRenderTarget) {
            preFilterRenderTarget->cleanup();
            preFilterRenderTarget.reset();
        }
    }

    void BloomEffectApplier::updateConfig(const Config& config) {
        if (this->config.maxIterations != config.maxIterations ||
                this->config.textureFetchQuality != config.textureFetchQuality) {

            this->config = config;
            checkConfig();

            refreshRenderers();
        }
    }

    const BloomEffectApplier::Config& BloomEffectApplier::getConfig() const {
        return config;
    }

    void BloomEffectApplier::checkConfig() const {
        if (config.maxIterations < 1) {
            throw std::invalid_argument("Bloom maximum iterations must be greater than zero");
        }
    }

    void BloomEffectApplier::applyBloom(uint32_t frameIndex, unsigned int numDependenciesToBloomCombineTexture) const {
        ScopeProfiler sp(Profiler::graphic(), "applyBloom");

        unsigned int numDependenciesToPreFilterOutput = 2; //first down sample & last up sample
        preFilterRenderTarget->render(frameIndex, numDependenciesToPreFilterOutput);

        for (const auto& downSampleRenderTarget : downSampleRenderTargets) {
            unsigned int numDependenciesToDownSampleOutput = 1; //next down sample OR first up sample
            if (downSampleRenderTarget.get() != downSampleRenderTargets.back().get()) {
                numDependenciesToDownSampleOutput += 1; //up sample
            }
            downSampleRenderTarget->render(frameIndex, numDependenciesToDownSampleOutput);
        }

        for (const auto& upSampleRenderTarget : upSampleRenderTargets) {
            unsigned int numDependenciesToUpSampleOutput = 1; //next up sample OR bloom combine
            upSampleRenderTarget->render(frameIndex, numDependenciesToUpSampleOutput);
        }

        combineRenderTarget->render(frameIndex, numDependenciesToBloomCombineTexture);
    }

}