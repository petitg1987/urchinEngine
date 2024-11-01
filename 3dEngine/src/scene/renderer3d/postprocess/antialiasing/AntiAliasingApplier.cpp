#include <scene/renderer3d/VisualConfig.h>
#include <scene/renderer3d/postprocess/antialiasing/AntiAliasingApplier.h>
#include <graphics/render/shader/ShaderBuilder.h>
#include <graphics/render/GenericRendererBuilder.h>

namespace urchin {

    AntiAliasingApplier::AntiAliasingApplier(const Config& config, bool isTestMode) :
            isTestMode(isTestMode),
            config(config) {

    }

    AntiAliasingApplier::~AntiAliasingApplier() {
        clearRenderer();
    }

    void AntiAliasingApplier::refreshInputTexture(const std::shared_ptr<Texture>& inputTexture) {
        if (inputTexture.get() != this->inputTexture.get()) {
            this->invSceneSize = Point2(1.0f / (float) inputTexture->getWidth(), 1.0f / (float) inputTexture->getHeight());
            this->inputTexture = inputTexture;

            clearRenderer();
            outputTexture = Texture::build("anti aliased", inputTexture->getWidth(), inputTexture->getHeight(), VisualConfig::SCENE_TEXTURE_FORMAT);
            renderTarget = std::make_unique<OffscreenRender>("anti aliasing", isTestMode, RenderTarget::NO_DEPTH_ATTACHMENT);
            renderTarget->addOutputTexture(outputTexture);
            renderTarget->initialize();

            createOrUpdateRenderer();
        }
    }

    const std::shared_ptr<Texture>& AntiAliasingApplier::getOutputTexture() const {
        return outputTexture;
    }

    void AntiAliasingApplier::clearRenderer() {
        renderer.reset();

        if (renderTarget) {
            renderTarget->cleanup();
            renderTarget.reset();
        }
    }

    void AntiAliasingApplier::createOrUpdateRenderer() {
        createOrUpdateFxaaShader();

        std::vector vertexCoord = {
                Point2(-1.0f, -1.0f), Point2(1.0f, -1.0f), Point2(1.0f, 1.0f),
                Point2(-1.0f, -1.0f), Point2(1.0f, 1.0f), Point2(-1.0f, 1.0f)
        };
        std::vector textureCoord = {
                Point2(0.0f, 0.0f), Point2(1.0f, 0.0f), Point2(1.0f, 1.0f),
                Point2(0.0f, 0.0f), Point2(1.0f, 1.0f), Point2(0.0f, 1.0f)
        };
        renderer = GenericRendererBuilder::create("anti aliasing", *renderTarget, *fxaaShader, ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformData(INV_SCENE_SIZE_UNIFORM_BINDING, sizeof(invSceneSize), &invSceneSize)
                ->addUniformTextureReader(INPUT_TEX_UNIFORM_BINDING, TextureReader::build(inputTexture, TextureParam::buildLinear()))
                ->build();
    }

    void AntiAliasingApplier::createOrUpdateFxaaShader() {
        AntiAliasingShaderConst antiAliasingShaderConst = {};
        if (config.quality == LOW) {
            antiAliasingShaderConst = {6, 1.0f, 1.5f, 2.0f, 2.0f, 4.0f, 12.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f};
        } else if (config.quality == MEDIUM) {
            antiAliasingShaderConst = {8, 1.0f, 1.5f, 2.0f, 2.0f, 2.0f, 2.0f, 4.0f, 8.0f, -1.0f, -1.0f, -1.0f, -1.0f};
        } else if (config.quality == HIGH) {
            antiAliasingShaderConst = {12, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.5f, 2.0f, 2.0f, 2.0f, 2.0f, 4.0f, 8.0f};
        }

        std::vector variablesSize = {
                sizeof(AntiAliasingShaderConst::qualityPs),
                sizeof(AntiAliasingShaderConst::qualityP0),
                sizeof(AntiAliasingShaderConst::qualityP1),
                sizeof(AntiAliasingShaderConst::qualityP2),
                sizeof(AntiAliasingShaderConst::qualityP3),
                sizeof(AntiAliasingShaderConst::qualityP4),
                sizeof(AntiAliasingShaderConst::qualityP5),
                sizeof(AntiAliasingShaderConst::qualityP6),
                sizeof(AntiAliasingShaderConst::qualityP7),
                sizeof(AntiAliasingShaderConst::qualityP8),
                sizeof(AntiAliasingShaderConst::qualityP9),
                sizeof(AntiAliasingShaderConst::qualityP10),
                sizeof(AntiAliasingShaderConst::qualityP11)
        };
        auto shaderConstants = std::make_unique<ShaderConstants>(variablesSize, &antiAliasingShaderConst);
        fxaaShader = ShaderBuilder::createShader("fxaa.vert.spv", "fxaa.frag.spv", std::move(shaderConstants), renderTarget->isTestMode());
    }

    void AntiAliasingApplier::updateConfig(const Config& config) {
        if (this->config.quality != config.quality) {
            this->config = config;

            createOrUpdateRenderer();
        }
    }

    const AntiAliasingApplier::Config& AntiAliasingApplier::getConfig() const {
        return config;
    }

    void AntiAliasingApplier::applyAntiAliasing(std::uint32_t frameIndex, unsigned int numDependenciesToAATexture) const {
        ScopeProfiler sp(Profiler::graphic(), "applyAA");

        renderTarget->render(frameIndex, numDependenciesToAATexture);
    }

}
