#include <map>

#include "scene/renderer3d/postprocess/antialiasing/AntiAliasingManager.h"
#include "graphic/render/shader/builder/ShaderBuilder.h"
#include "graphic/render/GenericRendererBuilder.h"

namespace urchin {

    AntiAliasingManager::AntiAliasingManager(std::shared_ptr<RenderTarget> renderTarget) :
            renderTarget(std::move(renderTarget)),
            config({}) {
        loadFxaaShader();
    }

    void AntiAliasingManager::loadFxaaShader() {
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

    void AntiAliasingManager::onTextureUpdate(const std::shared_ptr<Texture>& texture) {
        std::vector<Point2<float>> vertexCoord = {
                Point2<float>(-1.0f, -1.0f), Point2<float>(1.0f, -1.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(-1.0f, -1.0f), Point2<float>(1.0f, 1.0f), Point2<float>(-1.0f, 1.0f)
        };
        std::vector<Point2<float>> textureCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 0.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(0.0f, 1.0f)
        };
        renderer = GenericRendererBuilder::create("anti aliasing", renderTarget, fxaaShader, ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformData(sizeof(invSceneSize), &invSceneSize) //binding 0
                ->addUniformTextureReader(TextureReader::build(texture, TextureParam::buildLinear())) //binding 1
                ->build();
    }

    void AntiAliasingManager::onResize(unsigned int sceneWidth, unsigned int sceneHeight) {
        invSceneSize = Point2<float>(1.0f / (float)sceneWidth, 1.0f / (float)sceneHeight);
        if (renderer) {
            renderer->updateUniformData(0, &invSceneSize);
        }
    }

    void AntiAliasingManager::updateConfiguration(const AntiAliasingConfig& config) {
        if(this->config.quality != config.quality) {
            this->config = config;

            loadFxaaShader();
        }
    }

    void AntiAliasingManager::applyAntiAliasing() {
        if (renderer) {
            renderer->enableRenderer();
        }
    }

}
