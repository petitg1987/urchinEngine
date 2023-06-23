#include <scene/renderer3d/postprocess/bloom/BloomEffectApplier.h>
#include <graphics/render/shader/ShaderBuilder.h>
#include <graphics/render/GenericRendererBuilder.h>
#include <graphics/render/GenericComputeBuilder.h>
#include <graphics/render/target/NullRenderTarget.h>

namespace urchin {

    BloomEffectApplier::BloomEffectApplier(const Config& config, RenderTarget& outputRenderTarget, float gammaFactor) :
            config(config),
            outputRenderTarget(outputRenderTarget),
            sceneWidth(0),
            sceneHeight(0),
            preFilterTweak({}),
            gammaFactor(gammaFactor) {
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

    void BloomEffectApplier::onTextureUpdate(const std::shared_ptr<Texture>& inputHdrTexture) {
        this->inputHdrTexture = inputHdrTexture;
        this->sceneWidth = inputHdrTexture->getWidth();
        this->sceneHeight = inputHdrTexture->getHeight();

        refreshRenderers();
    }

    void BloomEffectApplier::onGammaFactorUpdate(float gammaFactor) {
        this->gammaFactor = gammaFactor;

        if (combineRenderer) {
            combineRenderer->updateUniformData(1, &gammaFactor);
        }
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
        std::vector<Point2<float>> vertexCoord = {
                Point2<float>(-1.0f, -1.0f), Point2<float>(1.0f, -1.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(-1.0f, -1.0f), Point2<float>(1.0f, 1.0f), Point2<float>(-1.0f, 1.0f)
        };
        std::vector<Point2<float>> textureCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 0.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(0.0f, 1.0f)
        };

        //shader constants
        BloomShadersConst bloomShadersConst = {};
        bloomShadersConst.qualityTextureFetch = (uint32_t)config.textureFetchQuality;
        bloomShadersConst.upSampleScale = UP_SAMPLE_SCALE;
        std::vector<std::size_t> downSampleVarSize = {sizeof(bloomShadersConst.qualityTextureFetch)};
        std::vector<std::size_t> upSampleVarSize = {sizeof(bloomShadersConst.qualityTextureFetch), sizeof(bloomShadersConst.upSampleScale)};

        //pre-filter
        preFilterShader = createComputeShader("bloomPreFilter.comp.spv", std::make_unique<ShaderConstants>(downSampleVarSize, &bloomShadersConst));
        if (outputRenderTarget.isValidRenderTarget()) {
            preFilterRenderTarget = std::make_unique<OffscreenRender>("bloom - pre filter", RenderTarget::NO_DEPTH_ATTACHMENT);
            static_cast<OffscreenRender*>(preFilterRenderTarget.get())->addOutputTexture(bloomStepTextures[0], LoadType::NO_LOAD, std::nullopt, OutputUsage::COMPUTE);
            preFilterRenderTarget->initialize();
        } else {
            preFilterShader = ShaderBuilder::createNullShader();
            preFilterRenderTarget = std::make_unique<NullRenderTarget>(bloomStepTextures[0]->getWidth(), bloomStepTextures[0]->getHeight());
        }

        Point2 texelSize(1.0f / (float)inputHdrTexture->getWidth(), 1.0f / (float)inputHdrTexture->getHeight());
        preFilterCompute = GenericComputeBuilder::create("bloom - pre filter", *preFilterRenderTarget, *preFilterShader, Vector2<int>(8, 8))
                ->addUniformData(sizeof(preFilterTweak), &preFilterTweak) //binding 0
                ->addUniformData(sizeof(texelSize), &texelSize) //binding 1
                ->addUniformTextureReader(TextureReader::build(inputHdrTexture, TextureParam::buildLinear())) //binding 2
                ->addUniformTextureOutput(bloomStepTextures[0]) //binding 3
                ->build();

        //down sample
        downSampleShader = createComputeShader("bloomDownSample.comp.spv", std::make_unique<ShaderConstants>(downSampleVarSize, &bloomShadersConst));
        for (std::size_t outTexIndex = 1, i = 0; outTexIndex < bloomStepTextures.size(); ++outTexIndex, ++i) {
            std::size_t srcTexIndex = outTexIndex - 1;

            std::unique_ptr<RenderTarget> downSampleRenderTarget;
            if (outputRenderTarget.isValidRenderTarget()) {
                downSampleRenderTarget = std::make_unique<OffscreenRender>("bloom - down sample " + std::to_string(i), RenderTarget::NO_DEPTH_ATTACHMENT);
                static_cast<OffscreenRender*>(downSampleRenderTarget.get())->addOutputTexture(bloomStepTextures[outTexIndex], LoadType::NO_LOAD, std::nullopt, OutputUsage::COMPUTE);
                downSampleRenderTarget->initialize();
            } else {
                downSampleRenderTarget = std::make_unique<NullRenderTarget>(bloomStepTextures[outTexIndex]->getWidth(), bloomStepTextures[outTexIndex]->getHeight());
            }

            Point2 texelSize(1.0f / (float)bloomStepTextures[srcTexIndex]->getWidth(), 1.0f / (float)bloomStepTextures[srcTexIndex]->getHeight());
            downSampleComputes.push_back(GenericComputeBuilder::create("bloom - down sample " + std::to_string(i), *downSampleRenderTarget, *downSampleShader, Vector2<int>(8, 8))
                    ->addUniformData(sizeof(texelSize), &texelSize) //binding 0
                    ->addUniformTextureReader(TextureReader::build(bloomStepTextures[srcTexIndex], TextureParam::buildLinear())) //binding 1
                    ->addUniformTextureOutput(bloomStepTextures[outTexIndex]) //binding 2
                    ->build());
            downSampleRenderTargets.push_back(std::move(downSampleRenderTarget));
        }

        //up sample
        upSampleShader = createShader("bloomUpSample.vert.spv", "bloomUpSample.frag.spv", std::make_unique<ShaderConstants>(upSampleVarSize, &bloomShadersConst));
        for (std::size_t srcTexIndex = bloomStepTextures.size() - 1, i = 0; srcTexIndex > 0; --srcTexIndex, ++i) {
            std::size_t outTexIndex = srcTexIndex - 1;

            std::unique_ptr<RenderTarget> upSampleRenderTarget;
            if (outputRenderTarget.isValidRenderTarget()) {
                upSampleRenderTarget = std::make_unique<OffscreenRender>("bloom - up sample " + std::to_string(i), RenderTarget::NO_DEPTH_ATTACHMENT);
                static_cast<OffscreenRender*>(upSampleRenderTarget.get())->addOutputTexture(bloomStepTextures[outTexIndex], LoadType::LOAD_CONTENT);
                upSampleRenderTarget->initialize();
            } else {
                upSampleRenderTarget = std::make_unique<NullRenderTarget>(bloomStepTextures[outTexIndex]->getWidth(), bloomStepTextures[outTexIndex]->getHeight());
            }

            Point2 texelSize(1.0f / (float)bloomStepTextures[srcTexIndex]->getWidth(), 1.0f / (float)bloomStepTextures[srcTexIndex]->getHeight());
            upSampleRenderers.push_back(GenericRendererBuilder::create("bloom - up sample " + std::to_string(i), *upSampleRenderTarget, *upSampleShader, ShapeType::TRIANGLE)
                    ->addData(vertexCoord)
                    ->addData(textureCoord)
                    ->addUniformData(sizeof(texelSize), &texelSize) //binding 0
                    ->addUniformTextureReader(TextureReader::build(bloomStepTextures[srcTexIndex], TextureParam::buildLinear())) //binding 1
                    ->enableTransparency({BlendFunction::build(BlendFactor::ONE, BlendFactor::ONE, BlendFactor::ONE, BlendFactor::ONE)})
                    ->build());
            upSampleRenderTargets.push_back(std::move(upSampleRenderTarget));
        }

        //combine
        combineShader = createShader("bloomCombine.vert.spv", "bloomCombine.frag.spv", std::make_unique<ShaderConstants>(upSampleVarSize, &bloomShadersConst));
        texelSize = Point2<float>(1.0f / (float)bloomStepTextures[0]->getWidth(), 1.0f / (float)bloomStepTextures[0]->getHeight());
        combineRenderer = GenericRendererBuilder::create("bloom - combine", outputRenderTarget, *combineShader, ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformData(sizeof(texelSize), &texelSize) //binding 0
                ->addUniformData(sizeof(gammaFactor), &gammaFactor) //binding 1
                ->addUniformTextureReader(TextureReader::build(bloomStepTextures[0], TextureParam::buildLinear())) //binding 2
                ->addUniformTextureReader(TextureReader::build(inputHdrTexture, TextureParam::buildLinear())) //binding 3
                ->build();
    }

    void BloomEffectApplier::clearRenderers() {
        //combine
        combineRenderer.reset();

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

    std::unique_ptr<Shader> BloomEffectApplier::createShader(const std::string& vertexShaderFilename, const std::string& fragmentShaderFilename, std::unique_ptr<ShaderConstants> shaderConstants) {
        if (outputRenderTarget.isValidRenderTarget()) {
            return ShaderBuilder::createShader(vertexShaderFilename, "", fragmentShaderFilename, std::move(shaderConstants));
        } else {
            return ShaderBuilder::createNullShader();
        }
    }

    std::unique_ptr<Shader> BloomEffectApplier::createComputeShader(const std::string& computeShaderFilename, std::unique_ptr<ShaderConstants> shaderConstants) {
        if (outputRenderTarget.isValidRenderTarget()) {
            return ShaderBuilder::createComputeShader(computeShaderFilename, std::move(shaderConstants));
        } else {
            return ShaderBuilder::createNullShader();
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

    void BloomEffectApplier::applyBloom(std::uint32_t frameIndex, unsigned int renderingOrder) const {
        ScopeProfiler sp(Profiler::graphic(), "applyBloom");

        unsigned int numDependenciesToPreFilterOutput = 1 /* first down sample */;
        preFilterRenderTarget->render(frameIndex, numDependenciesToPreFilterOutput);

        for (const auto& downSampleRenderTarget : downSampleRenderTargets) {
            unsigned int numDependenciesToDownSampleOutput = 1 /*next down sample OR first up sample */;
            downSampleRenderTarget->render(frameIndex, numDependenciesToDownSampleOutput);
        }

        for(const auto & upSampleRenderTarget : upSampleRenderTargets) {
            unsigned int numDependenciesToUpSampleOutput = 1 /* next up sample OR bloom combine (screen target) */;
            upSampleRenderTarget->render(frameIndex, numDependenciesToUpSampleOutput);
        }

        combineRenderer->enableRenderer(renderingOrder);
    }

}