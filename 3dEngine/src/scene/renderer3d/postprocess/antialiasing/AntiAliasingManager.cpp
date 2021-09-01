#include <map>

#include <scene/renderer3d/postprocess/antialiasing/AntiAliasingManager.h>
#include <graphic/render/shader/builder/ShaderBuilder.h>
#include <graphic/render/GenericRendererBuilder.h>

namespace urchin {

    AntiAliasingManager::AntiAliasingManager() :
            config({}) {

    }

    AntiAliasingManager::~AntiAliasingManager() {
        clearRenderer();
    }

    void AntiAliasingManager::onTextureUpdate(const std::shared_ptr<Texture>& inputTexture) {
        this->invSceneSize = Point2<float>(1.0f / (float)inputTexture->getWidth(), 1.0f / (float)inputTexture->getHeight());
        this->inputTexture = inputTexture;

        clearRenderer();
        outputTexture = Texture::build(inputTexture->getWidth(), inputTexture->getHeight(), TextureFormat::B10G11R11_FLOAT, nullptr);
        renderTarget = std::make_unique<OffscreenRender>("anti aliasing", RenderTarget::NO_DEPTH_ATTACHMENT);
        renderTarget->addOutputTexture(outputTexture);
        renderTarget->initialize();

        createOrUpdateRenderer();
    }

    const std::shared_ptr<Texture>& AntiAliasingManager::getOutputTexture() const {
        return outputTexture;
    }

    void AntiAliasingManager::clearRenderer() {
        if (renderer) {
            renderer.reset();
        }
        if (renderTarget) {
            renderTarget->cleanup();
            renderTarget.reset();
        }
    }

    void AntiAliasingManager::createOrUpdateRenderer() {
        createOrUpdateFxaaShader();

        std::vector<Point2<float>> vertexCoord = {
                Point2<float>(-1.0f, -1.0f), Point2<float>(1.0f, -1.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(-1.0f, -1.0f), Point2<float>(1.0f, 1.0f), Point2<float>(-1.0f, 1.0f)
        };
        std::vector<Point2<float>> textureCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 0.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(0.0f, 1.0f)
        };
        renderer = GenericRendererBuilder::create("anti aliasing", *renderTarget, *fxaaShader, ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformData(sizeof(invSceneSize), &invSceneSize) //binding 0
                ->addUniformTextureReader(TextureReader::build(inputTexture, TextureParam::buildLinear())) //binding 1
                ->build();
    }

    void AntiAliasingManager::createOrUpdateFxaaShader() {
        AntiAliasingShaderConst antiAliasingShaderConst = {};
        if (config.quality == LOW) {
            antiAliasingShaderConst = {6, 1.0f, 1.5f, 2.0f, 2.0f, 4.0f, 12.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f};
        } else if (config.quality == MEDIUM) {
            antiAliasingShaderConst = {8, 1.0f, 1.5f, 2.0f, 2.0f, 2.0f, 2.0f, 4.0f, 8.0f, -1.0f, -1.0f, -1.0f, -1.0f};
        } else if (config.quality == HIGH) {
            antiAliasingShaderConst = {12, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.5f, 2.0f, 2.0f, 2.0f, 2.0f, 4.0f, 8.0f};
        }

        std::vector<std::size_t> variablesSize = {
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

        fxaaShader = ShaderBuilder::createShader("fxaa.vert.spv", "", "fxaa.frag.spv", std::move(shaderConstants));
    }

    void AntiAliasingManager::updateConfig(const Config& config) {
        if (this->config.quality != config.quality) {
            this->config = config;

            createOrUpdateRenderer();
        }
    }

    const AntiAliasingManager::Config& AntiAliasingManager::getConfig() const {
        return config;
    }

    void AntiAliasingManager::applyAntiAliasing() {
        renderTarget->render();
    }

}
