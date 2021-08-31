#include <scene/renderer3d/postprocess/bloom/BloomEffectApplier.h>
#include <graphic/render/target/OffscreenRender.h>
#include <graphic/render/GenericRendererBuilder.h>
#include <graphic/render/shader/builder/ShaderBuilder.h>

namespace urchin {

    BloomEffectApplier::BloomEffectApplier(RenderTarget& outputRenderTarget) :
            outputRenderTarget(outputRenderTarget),
            config({}),
            sceneWidth(0),
            sceneHeight(0),
            preFilterTweak({}) {
        preFilterTweak.threshold = ConfigService::instance().getFloatValue("bloom.threshold");
    }

    BloomEffectApplier::~BloomEffectApplier() {
        clearRenderers();
    }

    void BloomEffectApplier::onTextureUpdate(const std::shared_ptr<Texture>& inputHdrTexture) {
        this->inputHdrTexture = inputHdrTexture;
        this->sceneWidth = inputHdrTexture->getWidth();
        this->sceneHeight = inputHdrTexture->getHeight();

        clearRenderers();

        refreshRenderers();
    }

    void BloomEffectApplier::refreshRenderers() {
        //clear existing
        clearRenderers();

        //create bloom step textures
        unsigned int textureWidth = sceneWidth / 2;
        unsigned int textureHeight = sceneHeight / 2;
        bloomStepTextures.clear();
        for(unsigned int i = 0; i < config.maxIterations; ++i) {
            bloomStepTextures.push_back(Texture::build(textureWidth, textureHeight, TextureFormat::B10G11R11_FLOAT, nullptr));

            textureWidth = textureWidth / 2;
            textureHeight = textureHeight / 2;
            if (textureWidth < 2 || textureHeight < 2) {
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
        BloomShadersConst bloomShadersConst{};
        bloomShadersConst.qualityTextureFetch = (config.textureFetchQuality == TextureFetchQuality::QUALITY_FETCH);
        bloomShadersConst.upSampleScale = ConfigService::instance().getFloatValue("bloom.upSampleScale");
        std::vector<std::size_t> downSampleVarSize = {sizeof(bloomShadersConst.qualityTextureFetch)};
        std::vector<std::size_t> upSampleVarSize = {sizeof(bloomShadersConst.qualityTextureFetch), sizeof(bloomShadersConst.upSampleScale)};

        //pre-filter
        preFilterShader = ShaderBuilder::createShader("bloomPreFilter.vert.spv", "", "bloomPreFilter.frag.spv", std::make_unique<ShaderConstants>(downSampleVarSize, &bloomShadersConst));
        preFilterRenderTarget = std::make_unique<OffscreenRender>("bloom - pre filter", RenderTarget::NO_DEPTH_ATTACHMENT);
        preFilterRenderTarget->addOutputTexture(bloomStepTextures[0]);
        preFilterRenderTarget->initialize();

        Point2<float> texelSize(1.0f / (float)inputHdrTexture->getWidth(), 1.0f / (float)inputHdrTexture->getHeight());
        preFilterRenderer = GenericRendererBuilder::create("bloom - pre filter", *preFilterRenderTarget, *preFilterShader, ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformData(sizeof(preFilterTweak), &preFilterTweak) //binding 0
                ->addUniformData(sizeof(texelSize), &texelSize) //binding 1
                ->addUniformTextureReader(TextureReader::build(inputHdrTexture, TextureParam::buildLinear())) //binding 2
                ->build();

        //down sample
        downSampleShader = ShaderBuilder::createShader("bloomDownSample.vert.spv", "", "bloomDownSample.frag.spv", std::make_unique<ShaderConstants>(downSampleVarSize, &bloomShadersConst));
        for(std::size_t outTexIndex = 1, i = 0; outTexIndex < bloomStepTextures.size(); ++outTexIndex, ++i) {
            std::size_t srcTexIndex = outTexIndex - 1;

            auto downSampleRenderTarget = std::make_unique<OffscreenRender>("bloom - down sample " + std::to_string(i), RenderTarget::NO_DEPTH_ATTACHMENT);
            downSampleRenderTarget->addOutputTexture(bloomStepTextures[outTexIndex]);
            downSampleRenderTarget->initialize();

            Point2<float> texelSize(1.0f / (float)bloomStepTextures[srcTexIndex]->getWidth(), 1.0f / (float)bloomStepTextures[srcTexIndex]->getHeight());
            downSampleRenderers.push_back(GenericRendererBuilder::create("bloom - down sample " + std::to_string(i), *downSampleRenderTarget, *downSampleShader, ShapeType::TRIANGLE)
                    ->addData(vertexCoord)
                    ->addData(textureCoord)
                    ->addUniformData(sizeof(texelSize), &texelSize) //binding 0
                    ->addUniformTextureReader(TextureReader::build(bloomStepTextures[srcTexIndex], TextureParam::buildLinear())) //binding 1
                    ->build());
            downSampleRenderTargets.push_back(std::move(downSampleRenderTarget));
        }

        //up sample
        upSampleShader = ShaderBuilder::createShader("bloomUpSample.vert.spv", "", "bloomUpSample.frag.spv", std::make_unique<ShaderConstants>(upSampleVarSize, &bloomShadersConst));
        for(std::size_t srcTexIndex = bloomStepTextures.size() - 1, i = 0; srcTexIndex > 0; --srcTexIndex, ++i) {
            std::size_t outTexIndex = srcTexIndex - 1;

            auto upSampleRenderTarget = std::make_unique<OffscreenRender>("bloom - up sample " + std::to_string(i), RenderTarget::NO_DEPTH_ATTACHMENT);
            upSampleRenderTarget->addOutputTexture(bloomStepTextures[outTexIndex], LoadType::LOAD_CONTENT);
            upSampleRenderTarget->initialize();

            Point2<float> texelSize(1.0f / (float)bloomStepTextures[srcTexIndex]->getWidth(), 1.0f / (float)bloomStepTextures[srcTexIndex]->getHeight());
            upSampleRenderers.push_back(GenericRendererBuilder::create("bloom - up sample " + std::to_string(i), *upSampleRenderTarget, *upSampleShader, ShapeType::TRIANGLE)
                    ->addData(vertexCoord)
                    ->addData(textureCoord)
                    ->addUniformData(sizeof(texelSize), &texelSize) //binding 0
                    ->addUniformTextureReader(TextureReader::build(bloomStepTextures[srcTexIndex], TextureParam::buildLinear())) //binding 1
                    ->enableTransparency({BlendFunction::build(ONE, ONE, ONE, ONE)})
                    ->build());
            upSampleRenderTargets.push_back(std::move(upSampleRenderTarget));
        }

        //combine
        combineShader = ShaderBuilder::createShader("bloomCombine.vert.spv", "", "bloomCombine.frag.spv", std::make_unique<ShaderConstants>(upSampleVarSize, &bloomShadersConst));
        texelSize = Point2<float>(1.0f / (float)bloomStepTextures[0]->getWidth(), 1.0f / (float)bloomStepTextures[0]->getHeight());
        combineRenderer = GenericRendererBuilder::create("bloom - combine", outputRenderTarget, *combineShader, ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformData(sizeof(texelSize), &texelSize) //binding 0
                ->addUniformTextureReader(TextureReader::build(bloomStepTextures[0], TextureParam::buildLinear())) //binding 1
                ->addUniformTextureReader(TextureReader::build(inputHdrTexture, TextureParam::buildLinear())) //binding 2
                ->build();
    }

    void BloomEffectApplier::clearRenderers() {
        //combine
        combineRenderer.reset();

        //up sample
        upSampleRenderers.clear();
        for(auto& upSampleRenderTarget : upSampleRenderTargets) {
            upSampleRenderTarget->cleanup();
        }
        upSampleRenderTargets.clear();

        //down sample
        downSampleRenderers.clear();
        for(auto& downSampleRenderTarget : downSampleRenderTargets) {
            downSampleRenderTarget->cleanup();
        }
        downSampleRenderTargets.clear();

        //pre filter
        preFilterRenderer.reset();
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
            throw std::invalid_argument("Bloom maximum iterations must be greater than one");
        }
    }

    void BloomEffectApplier::applyBloom() {
        preFilterRenderTarget->render();
        for(auto& downSampleRenderTarget : downSampleRenderTargets) {
            downSampleRenderTarget->render();
        }
        for(auto& upSampleRenderTarget : upSampleRenderTargets) {
            upSampleRenderTarget->render();
        }

        combineRenderer->enableRenderer();
    }

}