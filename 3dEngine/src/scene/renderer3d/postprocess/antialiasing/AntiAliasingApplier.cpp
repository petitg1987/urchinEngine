#include <scene/renderer3d/VisualConfig.h>
#include <scene/renderer3d/postprocess/antialiasing/AntiAliasingApplier.h>
#include <graphics/render/shader/ShaderBuilder.h>
#include <graphics/render/GenericRendererBuilder.h>

namespace urchin {

    AntiAliasingApplier::AntiAliasingApplier(const Config& config, bool isTestMode) :
            isTestMode(isTestMode),
            config(config),
            frameCount(0) {

    }

    AntiAliasingApplier::~AntiAliasingApplier() {
        clearRenderer();
    }

    void AntiAliasingApplier::applyCameraJitter(Camera& camera) {
        constexpr std::array HALTON_SEQUENCE_X = {0.500000f, 0.250000f, 0.750000f, 0.125000f, 0.625000f, 0.375000f, 0.875000f, 0.062500f,
            0.562500f, 0.312500f, 0.812500f, 0.187500f, 0.687500f, 0.437500f, 0.937500f, 0.031250f};
        constexpr std::array HALTON_SEQUENCE_Y = {0.333333f, 0.666667f, 0.111111f, 0.444444f, 0.777778f, 0.222222f, 0.555556f, 0.888889f,
            0.037037f, 0.370370f, 0.703704f, 0.148148f, 0.481481f, 0.814815f, 0.259259f, 0.592593f};
        constexpr unsigned int JITTER_PERIOD_UPDATE = 4; //update the jitter values only on every 'x' frame

        std::size_t sequenceIndex = (frameCount % (HALTON_SEQUENCE_X.size() * JITTER_PERIOD_UPDATE)) / JITTER_PERIOD_UPDATE;
        float valueX = HALTON_SEQUENCE_X[sequenceIndex] / (float)inputTexture->getWidth();
        float valueY = HALTON_SEQUENCE_Y[sequenceIndex] / (float)inputTexture->getHeight();
        camera.applyJitter(valueX, valueY);

        frameCount++;
    }

    void AntiAliasingApplier::refreshInputTexture(const std::shared_ptr<Texture>& inputTexture) {
        if (inputTexture.get() != this->inputTexture.get()) {
            this->invSceneSize = Point2(1.0f / (float) inputTexture->getWidth(), 1.0f / (float) inputTexture->getHeight());
            this->inputTexture = inputTexture;

            clearRenderer();
            outputTexture = Texture::build("anti aliased", inputTexture->getWidth(), inputTexture->getHeight(), VisualConfig::SCENE_HDR_TEXTURE_FORMAT);
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

    void AntiAliasingApplier::applyAntiAliasing(uint32_t frameIndex, unsigned int numDependenciesToAATexture) const {
        ScopeProfiler sp(Profiler::graphic(), "applyAA");

        renderTarget->render(frameIndex, numDependenciesToAATexture);
    }

}
