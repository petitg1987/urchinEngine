#include <map>
#include <string>

#include "scene/renderer3d/postprocess/antialiasing/AntiAliasingManager.h"
#include "graphic/render/shader/builder/ShaderBuilder.h"
#include "graphic/render/GenericRendererBuilder.h"

#define DEFAULT_AA_QUALITY AntiAliasingManager::Quality::VERY_HIGH

namespace urchin {

    AntiAliasingManager::AntiAliasingManager(std::shared_ptr<RenderTarget> renderTarget) :
            renderTarget(std::move(renderTarget)),
            quality(DEFAULT_AA_QUALITY) {
        loadFxaaShader();
    }

    void AntiAliasingManager::loadFxaaShader() {
        std::map<std::string, std::string> fxaaTokens;
        fxaaTokens["FXAA_QUALITY"] = std::to_string((int)quality);

        fxaaShader = ShaderBuilder::createShader("fxaa.vert", "", "fxaa.frag", fxaaTokens);
    }

    void AntiAliasingManager::onTextureUpdate(const std::shared_ptr<Texture>& texture) {
        std::vector<Point2<float>> vertexCoord = {
                Point2<float>(-1.0f, 1.0f), Point2<float>(1.0f, 1.0f), Point2<float>(1.0f, -1.0f),
                Point2<float>(-1.0f, 1.0f), Point2<float>(1.0f, -1.0f), Point2<float>(-1.0f, -1.0f)
        };
        std::vector<Point2<float>> textureCoord = {
                Point2<float>(0.0f, 1.0f), Point2<float>(1.0f, 1.0f), Point2<float>(1.0f, 0.0f),
                Point2<float>(0.0f, 1.0f), Point2<float>(1.0f, 0.0f), Point2<float>(0.0f, 0.0f)
        };
        renderer = std::make_unique<GenericRendererBuilder>(renderTarget, fxaaShader, ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addShaderData(sizeof(invSceneSize), &invSceneSize) //binding 0
                ->addTextureReader(TextureReader::build(texture, TextureParam::buildLinear()))
                ->build();
    }

    void AntiAliasingManager::onResize(unsigned int sceneWidth, unsigned int sceneHeight) {
        invSceneSize = Point2<float>(1.0f / (float)sceneWidth, 1.0f / (float)sceneHeight);
        if (renderer) {
            renderer->updateShaderData(0, &invSceneSize);
        }
    }

    void AntiAliasingManager::setQuality(Quality quality) {
        this->quality = quality;

        loadFxaaShader();
    }

    void AntiAliasingManager::applyAntiAliasing() {
        if (renderer) {
            renderTarget->addRenderer(renderer.get()); //TODO review unique_ptr
        }
    }

}
