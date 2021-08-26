#include <scene/renderer3d/postprocess/bloom/BloomEffectApplier.h>
#include <graphic/render/target/OffscreenRender.h>
#include <graphic/render/GenericRendererBuilder.h>
#include <graphic/render/shader/builder/ShaderBuilder.h>

namespace urchin {

    BloomEffectApplier::BloomEffectApplier() :
            config({}),
            sceneWidth(0),
            sceneHeight(0),
            preFilterTweak({}),
            outputRenderTarget(nullptr) {
        preFilterTweak.threshold = ConfigService::instance().getFloatValue("bloom.threshold");
    }

    BloomEffectApplier::~BloomEffectApplier() {
        clearRenderers();
        clearOutputRenderTarget();
    }

    void BloomEffectApplier::onTextureUpdate(const std::shared_ptr<Texture>& inputHdrTexture, std::optional<RenderTarget*> customRenderTarget) {
        this->inputHdrTexture = inputHdrTexture;
        this->sceneWidth = inputHdrTexture->getWidth();
        this->sceneHeight = inputHdrTexture->getHeight();

        clearRenderers();
        clearOutputRenderTarget();

        if (customRenderTarget.has_value()) {
            this->outputRenderTarget = customRenderTarget.value();
        } else {
            outputLdrTexture = Texture::build(sceneWidth, sceneHeight, TextureFormat::RGBA_8_INT, nullptr);
            outputOffscreenRenderTarget = std::make_unique<OffscreenRender>("bloom - combine", RenderTarget::NO_DEPTH_ATTACHMENT);
            outputOffscreenRenderTarget.value()->resetTextures();
            outputOffscreenRenderTarget.value()->addTexture(outputLdrTexture.value());
            outputOffscreenRenderTarget.value()->initialize();

            this->outputRenderTarget = outputOffscreenRenderTarget.value().get();
        }
        refreshRenderers();
    }

    void BloomEffectApplier::clearOutputRenderTarget() {
        if (outputOffscreenRenderTarget.has_value()) {
            outputOffscreenRenderTarget.value()->cleanup();
            outputOffscreenRenderTarget.value().reset();
        }

        if(outputLdrTexture.has_value()) {
            outputLdrTexture.value().reset();
        }
    }

    void BloomEffectApplier::refreshRenderers() {
        //clear existing
        clearRenderers();

        //create bloom step textures
        unsigned int textureWidth = sceneWidth;
        unsigned int textureHeight = sceneHeight;
        bloomStepTextures.clear();
        for(unsigned int i = 0; i < config.maximumIterations; ++i) {
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

        //pre-filter
        preFilterRenderTarget = std::make_unique<OffscreenRender>("bloom - pre filter", RenderTarget::NO_DEPTH_ATTACHMENT);
        preFilterRenderTarget->resetTextures();
        preFilterRenderTarget->addTexture(bloomStepTextures[0]);
        preFilterRenderTarget->initialize();

        preFilterShader = ShaderBuilder::createShader("bloomPreFilter.vert.spv", "", "bloomPreFilter.frag.spv");
        preFilterRenderer = GenericRendererBuilder::create("bloom - pre filter", *preFilterRenderTarget, *preFilterShader, ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformData(sizeof(preFilterTweak), &preFilterTweak) //binding 0
                ->addUniformTextureReader(TextureReader::build(inputHdrTexture, TextureParam::buildLinear())) //binding 1
                ->build();

        //down sample //TODO iterate !
        downSampleRenderTarget = std::make_unique<OffscreenRender>("bloom - down sample", RenderTarget::NO_DEPTH_ATTACHMENT);
        downSampleRenderTarget->resetTextures();
        downSampleRenderTarget->addTexture(bloomStepTextures[1]);
        downSampleRenderTarget->initialize();

        downSampleShader = ShaderBuilder::createShader("bloomDownSample.vert.spv", "", "bloomDownSample.frag.spv");
        downSampleRenderer = GenericRendererBuilder::create("bloom - down sample", *downSampleRenderTarget, *downSampleShader, ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformTextureReader(TextureReader::build(bloomStepTextures[0], TextureParam::buildLinear())) //binding 0
                ->build();

        //up sample //TODO iterate !
        upSampleRenderTarget = std::make_unique<OffscreenRender>("bloom - up sample", RenderTarget::NO_DEPTH_ATTACHMENT);
        upSampleRenderTarget->resetTextures();
        upSampleRenderTarget->addTexture(bloomStepTextures[0]); //TODO cannot up sample on this texture because of blend
        upSampleRenderTarget->initialize();

        upSampleShader = ShaderBuilder::createShader("bloomUpSample.vert.spv", "", "bloomUpSample.frag.spv");
        upSampleRenderer = GenericRendererBuilder::create("bloom - up sample", *upSampleRenderTarget, *upSampleShader, ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformTextureReader(TextureReader::build(bloomStepTextures[1], TextureParam::buildLinear())) //binding 0
                ->enableTransparency({BlendFunction::build(ONE, ONE, ONE, ONE)}) //TODO check if it works
                ->build();

        //combine //TODO check ACES tone mapping
        combineShader = ShaderBuilder::createShader("bloomCombine.vert.spv", "", "bloomCombine.frag.spv");
        combineRenderer = GenericRendererBuilder::create("bloom - combine", *outputRenderTarget, *combineShader, ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformTextureReader(TextureReader::build(inputHdrTexture, TextureParam::buildLinear())) //binding 0
                ->addUniformTextureReader(TextureReader::build(bloomStepTextures[0], TextureParam::buildLinear())) //binding 1
                ->build();
    }

    void BloomEffectApplier::clearRenderers() {
        //combine
        combineRenderer.reset();

        //up sample
        upSampleRenderer.reset();
        if (upSampleRenderTarget) {
            upSampleRenderTarget->cleanup();
            upSampleRenderTarget.reset();
        }

        //down sample
        downSampleRenderer.reset();
        if (downSampleRenderTarget) {
            downSampleRenderTarget->cleanup();
            downSampleRenderTarget.reset();
        }

        //pre filter
        preFilterRenderer.reset();
        if (preFilterRenderTarget) {
            preFilterRenderTarget->cleanup();
            preFilterRenderTarget.reset();
        }
    }

    const std::shared_ptr<Texture>& BloomEffectApplier::getOutputTexture() const {
        if (!outputLdrTexture.has_value()) {
            throw std::runtime_error("No output texture available because rendering has been done in a custom render target");
        }
        return outputLdrTexture.value();
    }

    void BloomEffectApplier::updateConfig(const Config& config) {
        if (this->config.maximumIterations != config.maximumIterations ||
                this->config.fetchTextureQuality != config.fetchTextureQuality) {

            this->config = config;
            checkConfig();

            refreshRenderers();
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
        preFilterRenderTarget->render();
        downSampleRenderTarget->render();
        upSampleRenderTarget->render();

        if (outputOffscreenRenderTarget.has_value()) {
            outputOffscreenRenderTarget.value()->render();
        } else {
            combineRenderer->enableRenderer();
        }
    }

}