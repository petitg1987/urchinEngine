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
            outputOffscreenRenderTarget.value()->resetOutputTextures();
            outputOffscreenRenderTarget.value()->addOutputTexture(outputLdrTexture.value());
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
        preFilterShader = ShaderBuilder::createShader("bloomPreFilter.vert.spv", "", "bloomPreFilter.frag.spv");
        preFilterRenderTarget = std::make_unique<OffscreenRender>("bloom - pre filter", RenderTarget::NO_DEPTH_ATTACHMENT);
        preFilterRenderTarget->resetOutputTextures();
        preFilterRenderTarget->addOutputTexture(bloomStepTextures[0]);
        preFilterRenderTarget->initialize();

        preFilterRenderer = GenericRendererBuilder::create("bloom - pre filter", *preFilterRenderTarget, *preFilterShader, ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformData(sizeof(preFilterTweak), &preFilterTweak) //binding 0
                ->addUniformTextureReader(TextureReader::build(inputHdrTexture, TextureParam::buildLinear())) //binding 1
                ->build();

        //down sample
        //TODO down/up sample: change texture read from bilinear to nearest ? (depend of blur method)
        downSampleShader = ShaderBuilder::createShader("bloomDownSample.vert.spv", "", "bloomDownSample.frag.spv");
        for(std::size_t texIndex = 1, i = 0; texIndex < bloomStepTextures.size(); ++texIndex, ++i) {
            auto downSampleRenderTarget = std::make_unique<OffscreenRender>("bloom - down sample " + std::to_string(i), RenderTarget::NO_DEPTH_ATTACHMENT);
            downSampleRenderTarget->resetOutputTextures();
            downSampleRenderTarget->addOutputTexture(bloomStepTextures[texIndex]);
            downSampleRenderTarget->initialize();

            downSampleRenderers.push_back(GenericRendererBuilder::create("bloom - down sample " + std::to_string(i), *downSampleRenderTarget, *downSampleShader, ShapeType::TRIANGLE)
                    ->addData(vertexCoord)
                    ->addData(textureCoord)
                    ->addUniformTextureReader(TextureReader::build(bloomStepTextures[texIndex - 1], TextureParam::buildLinear())) //binding 0
                    ->build());
            downSampleRenderTargets.push_back(std::move(downSampleRenderTarget));
        }

        //up sample
        //TODO up sample: possible to avoid to write in the fullscreen tex (bloomStepTextures[0]) ?
        upSampleShader = ShaderBuilder::createShader("bloomUpSample.vert.spv", "", "bloomUpSample.frag.spv");
        for(std::size_t texIndex = bloomStepTextures.size() - 1, i = 0; texIndex > 0; --texIndex, ++i) {
            auto upSampleRenderTarget = std::make_unique<OffscreenRender>("bloom - up sample " + std::to_string(i), RenderTarget::NO_DEPTH_ATTACHMENT);
            upSampleRenderTarget->resetOutputTextures();
            upSampleRenderTarget->addOutputTexture(bloomStepTextures[texIndex - 1], LoadType::LOAD_CONTENT);
            upSampleRenderTarget->initialize();

            upSampleRenderers.push_back(GenericRendererBuilder::create("bloom - up sample " + std::to_string(i), *upSampleRenderTarget, *upSampleShader, ShapeType::TRIANGLE)
                    ->addData(vertexCoord)
                    ->addData(textureCoord)
                    ->addUniformTextureReader(TextureReader::build(bloomStepTextures[texIndex], TextureParam::buildLinear())) //binding 0
                    ->enableTransparency({BlendFunction::build(ONE, ONE, ONE, ONE)})
                    ->build());
            upSampleRenderTargets.push_back(std::move(upSampleRenderTarget));
        }

        //combine
        //TODO combine: check to use ACES tone mapping
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
        for(auto& downSampleRenderTarget : downSampleRenderTargets) {
            downSampleRenderTarget->render();
        }
        for(auto& upSampleRenderTarget : upSampleRenderTargets) {
            upSampleRenderTarget->render();
        }

        if (outputOffscreenRenderTarget.has_value()) {
            outputOffscreenRenderTarget.value()->render();
        } else {
            combineRenderer->enableRenderer();
        }
    }

}