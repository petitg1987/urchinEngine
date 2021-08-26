#include <scene/renderer3d/postprocess/bloom/BloomEffectApplier.h>
#include <graphic/render/target/OffscreenRender.h>
#include <graphic/render/GenericRendererBuilder.h>
#include <graphic/render/shader/builder/ShaderBuilder.h>

namespace urchin {

    BloomEffectApplier::BloomEffectApplier() :
            config({}),
            sceneWidth(0),
            sceneHeight(0),
            finalRenderTarget(nullptr),
            bloomTweak({}) {
        bloomTweak.threshold = ConfigService::instance().getFloatValue("bloom.threshold");
    }

    BloomEffectApplier::~BloomEffectApplier() {
        clearRendering();
    }

    void BloomEffectApplier::onTextureUpdate(const std::shared_ptr<Texture>& lightingPassTexture, std::optional<RenderTarget*> customRenderTarget) {
        this->lightingPassTexture = lightingPassTexture;
        this->sceneWidth = lightingPassTexture->getWidth();
        this->sceneHeight = lightingPassTexture->getHeight();

        clearRendering();
        if (customRenderTarget.has_value()) {
            this->finalRenderTarget = customRenderTarget.value();
        } else {
            bloomedTexture = Texture::build(sceneWidth, sceneHeight, TextureFormat::RGBA_8_INT, nullptr);
            offscreenBloomRenderTarget = std::make_unique<OffscreenRender>("bloom - combine", RenderTarget::NO_DEPTH_ATTACHMENT);
            offscreenBloomRenderTarget->resetTextures();
            offscreenBloomRenderTarget->addTexture(bloomedTexture);
            offscreenBloomRenderTarget->initialize();

            this->finalRenderTarget = offscreenBloomRenderTarget.get();
        }

        refreshRenderer();
    }

    void BloomEffectApplier::refreshRenderer() {
        //create intermediate textures
        unsigned int textureWidth = sceneWidth;
        unsigned int textureHeight = sceneHeight;
        intermediateTextures.clear();
        for(unsigned int i = 0; i < config.maximumIterations; ++i) {
            intermediateTextures.push_back(Texture::build(textureWidth, textureHeight, TextureFormat::B10G11R11_FLOAT, nullptr));

            textureWidth = textureWidth / 2;
            textureHeight = textureHeight / 2;
            if (textureWidth < 2 || textureHeight < 2) {
                break;
            }
        }

        //pre-filter + blur
        bloomFilteredRenderTarget = std::make_unique<OffscreenRender>("bloom - filtered", RenderTarget::NO_DEPTH_ATTACHMENT);
        bloomFilteredRenderTarget->resetTextures();
        bloomFilteredRenderTarget->addTexture(intermediateTextures[0]);
        bloomFilteredRenderTarget->initialize();

        std::vector<Point2<float>> vertexCoord = {
                Point2<float>(-1.0f, -1.0f), Point2<float>(1.0f, -1.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(-1.0f, -1.0f), Point2<float>(1.0f, 1.0f), Point2<float>(-1.0f, 1.0f)
        };
        std::vector<Point2<float>> textureCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 0.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(0.0f, 1.0f)
        };
        bloomPreFilterShader = ShaderBuilder::createShader("bloomPreFilter.vert.spv", "", "bloomPreFilter.frag.spv");
        preFilterRenderer = GenericRendererBuilder::create("bloom - prefilter", *bloomFilteredRenderTarget, *bloomPreFilterShader, ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformData(sizeof(bloomTweak), &bloomTweak) //binding 0
                ->addUniformTextureReader(TextureReader::build(lightingPassTexture, TextureParam::buildLinear())) //binding 1
                ->build();

        //down sample //TODO iterate !
        bloomDownSampleRenderTarget = std::make_unique<OffscreenRender>("bloom - down sample", RenderTarget::NO_DEPTH_ATTACHMENT);
        bloomDownSampleRenderTarget->resetTextures();
        bloomDownSampleRenderTarget->addTexture(intermediateTextures[1]);
        bloomDownSampleRenderTarget->initialize();

        bloomDownSampleShader = ShaderBuilder::createShader("bloomDownSample.vert.spv", "", "bloomDownSample.frag.spv");
        downSampleRenderer = GenericRendererBuilder::create("bloom - down sample", *bloomDownSampleRenderTarget, *bloomDownSampleShader, ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformTextureReader(TextureReader::build(intermediateTextures[0], TextureParam::buildLinear())) //binding 0
                ->build();

        //up sample //TODO iterate !
        bloomUpSampleRenderTarget = std::make_unique<OffscreenRender>("bloom - up sample", RenderTarget::NO_DEPTH_ATTACHMENT);
        bloomUpSampleRenderTarget->resetTextures();
        bloomUpSampleRenderTarget->addTexture(intermediateTextures[0]);
        bloomUpSampleRenderTarget->initialize();

        bloomUpSampleShader = ShaderBuilder::createShader("bloomUpSample.vert.spv", "", "bloomUpSample.frag.spv");
        upSampleRenderer = GenericRendererBuilder::create("bloom - up sample", *bloomUpSampleRenderTarget, *bloomUpSampleShader, ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformTextureReader(TextureReader::build(intermediateTextures[1], TextureParam::buildLinear())) //binding 0
                ->build();
        //TODO: not clear target texture and use blend ONE, ONE

        //combine
        bloomCombineShader = ShaderBuilder::createShader("bloomCombine.vert.spv", "", "bloomCombine.frag.spv");
        combineRenderer = GenericRendererBuilder::create("bloom - combine", *finalRenderTarget, *bloomCombineShader, ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformTextureReader(TextureReader::build(lightingPassTexture, TextureParam::buildLinear())) //binding 0
                ->addUniformTextureReader(TextureReader::build(intermediateTextures[0], TextureParam::buildLinear())) //binding 1
                ->build();
        //TODO check ACES tone mapping
    }

    void BloomEffectApplier::clearRendering() {
        //combine
        combineRenderer.reset();
        if(offscreenBloomRenderTarget) {
            offscreenBloomRenderTarget->cleanup();
            offscreenBloomRenderTarget.reset();
        }
        bloomedTexture.reset();

        //up sample
        upSampleRenderer.reset();
        if(bloomUpSampleRenderTarget) {
            bloomUpSampleRenderTarget->cleanup();
            bloomUpSampleRenderTarget.reset();
        }

        //down sample
        downSampleRenderer.reset();
        if(bloomDownSampleRenderTarget) {
            bloomDownSampleRenderTarget->cleanup();
            bloomDownSampleRenderTarget.reset();
        }

        //pre filter
        preFilterRenderer.reset();
        if (bloomFilteredRenderTarget) {
            bloomFilteredRenderTarget->cleanup();
            bloomFilteredRenderTarget.reset();
        }
    }

    const std::shared_ptr<Texture>& BloomEffectApplier::getBloomedTexture() const {
        if (!bloomedTexture) {
            throw std::runtime_error("No bloomed texture available because rendering has been done in a custom render target");
        }
        return bloomedTexture;
    }

    void BloomEffectApplier::updateConfig(const Config& config) {
        if (this->config.maximumIterations != config.maximumIterations ||
                this->config.fetchTextureQuality != config.fetchTextureQuality) {

            this->config = config;
            checkConfig();

            refreshRenderer();
        }
    }

    const BloomEffectApplier::Config& BloomEffectApplier::getConfig() const {
        return config;
    }

    void BloomEffectApplier::checkConfig() const {
        if (config.maximumIterations < 1) {
            throw std::invalid_argument("Bloom maximum iterations must be greater than one");
        }
    }

    void BloomEffectApplier::applyBloom() {
        bloomFilteredRenderTarget->render();
        bloomDownSampleRenderTarget->render();
        bloomUpSampleRenderTarget->render();

        if (bloomedTexture) {
            finalRenderTarget->render();
        } else {
            combineRenderer->enableRenderer();
        }
    }

}