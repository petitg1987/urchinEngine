#include <scene/renderer3d/VisualConfig.h>
#include <scene/renderer3d/postprocess/antialiasing/FxaaApplier.h>
#include <graphics/render/shader/ShaderBuilder.h>
#include <graphics/render/GenericRendererBuilder.h>

namespace urchin {

    FxaaApplier::FxaaApplier(bool isTestMode, bool isEnabled, AntiAliasingQuality quality) :
            isTestMode(isTestMode),
            isEnabled(isEnabled),
            quality(quality) {

    }

    FxaaApplier::~FxaaApplier() {
        freeRenderData();
    }

    void FxaaApplier::enableApplier(bool enable) {
        this->isEnabled = enable;

        if (isEnabled) {
            createOrUpdateRenderData();
        } else {
            freeRenderData();
        }
    }

    void FxaaApplier::refreshInputTexture(const std::shared_ptr<Texture>& inputTexture) {
        if (inputTexture.get() != this->inputTexture.get()) {
            this->inputTexture = inputTexture;

            if (isEnabled) {
                createOrUpdateRenderData();
            }
        }
    }

    const std::shared_ptr<Texture>& FxaaApplier::getOutputTexture() const {
        return outputTexture;
    }

    void FxaaApplier::createOrUpdateRenderData() {
        freeRenderData();

        outputTexture = Texture::build("anti aliased", inputTexture->getWidth(), inputTexture->getHeight(), VisualConfig::SCENE_HDR_TEXTURE_FORMAT);
        renderTarget = std::make_unique<OffscreenRender>("anti aliasing", isTestMode, RenderTarget::NO_DEPTH_ATTACHMENT);
        renderTarget->addOutputTexture(outputTexture);
        renderTarget->initialize();
        createOrUpdateRenderer();
    }

    void FxaaApplier::freeRenderData() {
        renderer.reset();
        fxaaShader.reset();
        renderTarget.reset();
        outputTexture.reset();
    }

    void FxaaApplier::createOrUpdateRenderer() {
        createOrUpdateFxaaShader();

        std::vector vertexCoord = {
            Point2(-1.0f, -1.0f), Point2(1.0f, -1.0f), Point2(1.0f, 1.0f),
            Point2(-1.0f, -1.0f), Point2(1.0f, 1.0f), Point2(-1.0f, 1.0f)
        };
        std::vector textureCoord = {
            Point2(0.0f, 0.0f), Point2(1.0f, 0.0f), Point2(1.0f, 1.0f),
            Point2(0.0f, 0.0f), Point2(1.0f, 1.0f), Point2(0.0f, 1.0f)
        };
        Point2 invSceneSize(1.0f / (float) inputTexture->getWidth(), 1.0f / (float) inputTexture->getHeight());
        renderer = GenericRendererBuilder::create("anti aliasing", *renderTarget, *fxaaShader, ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformData(INV_SCENE_SIZE_UNIFORM_BINDING, sizeof(invSceneSize), &invSceneSize)
                ->addUniformTextureReader(INPUT_TEX_UNIFORM_BINDING, TextureReader::build(inputTexture, TextureParam::buildLinear()))
                ->build();
    }

    void FxaaApplier::createOrUpdateFxaaShader() {
        AntiAliasingShaderConst antiAliasingShaderConst = {};
        if (quality == AntiAliasingQuality::MEDIUM) {
            antiAliasingShaderConst = {8, 1.0f, 1.5f, 2.0f, 2.0f, 2.0f, 2.0f, 4.0f, 8.0f, -1.0f, -1.0f, -1.0f, -1.0f};
        } else if (quality == AntiAliasingQuality::HIGH) {
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

    void FxaaApplier::updateQuality(AntiAliasingQuality quality) {
        this->quality = quality;

        if (isEnabled) {
            createOrUpdateRenderer();
        }
    }

    void FxaaApplier::applyAntiAliasing(uint32_t frameCount, unsigned int numDependenciesToAATexture) const {
        assert(isEnabled);
        renderTarget->render(frameCount, numDependenciesToAATexture);
    }

}
