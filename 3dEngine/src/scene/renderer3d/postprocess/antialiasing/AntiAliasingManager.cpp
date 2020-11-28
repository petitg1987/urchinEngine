#include <map>
#include <string>

#include "scene/renderer3d/postprocess/antialiasing/AntiAliasingManager.h"
#include "graphic/shader/builder/ShaderBuilder.h"
#include "graphic/shader/data/ShaderDataSender.h"
#include "graphic/render/GenericRendererBuilder.h"

#define DEFAULT_AA_QUALITY AntiAliasingManager::Quality::VERY_HIGH

namespace urchin {

    AntiAliasingManager::AntiAliasingManager(const RenderTarget *renderTarget) :
            renderTarget(renderTarget),
            quality(DEFAULT_AA_QUALITY),
            sceneWidth(0),
            sceneHeight(0) {
        loadFxaaShader();
    }

    void AntiAliasingManager::loadFxaaShader() {
        std::map<std::string, std::string> fxaaTokens;
        fxaaTokens["FXAA_QUALITY"] = std::to_string(static_cast<int>(quality));

        fxaaShader = ShaderBuilder().createShader("fxaa.vert", "", "fxaa.frag", fxaaTokens);
        invSceneSizeShaderVar = ShaderVar(fxaaShader, "invSceneSize");

        ShaderDataSender()
                .sendData(ShaderVar(fxaaShader, "tex"), 0)
                .sendData(invSceneSizeShaderVar, Point2<float>(1.0f/(float)sceneWidth, 1.0f/(float)sceneHeight));
    }

    void AntiAliasingManager::onTextureUpdate(const std::shared_ptr<Texture> &texture) {
        std::vector<Point2<float>> vertexCoord = {
                Point2<float>(-1.0f, 1.0f), Point2<float>(1.0f, 1.0f), Point2<float>(1.0f, -1.0f),
                Point2<float>(-1.0f, 1.0f), Point2<float>(1.0f, -1.0f), Point2<float>(-1.0f, -1.0f)
        };
        std::vector<Point2<float>> textureCoord = {
                Point2<float>(0.0f, 1.0f), Point2<float>(1.0f, 1.0f), Point2<float>(1.0f, 0.0f),
                Point2<float>(0.0f, 1.0f), Point2<float>(1.0f, 0.0f), Point2<float>(0.0f, 0.0f)
        };
        renderer = std::make_unique<GenericRendererBuilder>(ShapeType::TRIANGLE)
                ->addData(&vertexCoord)
                ->addData(&textureCoord)
                ->addTexture(TextureReader::build(texture, TextureParam::buildLinear()))
                ->build();
    }

    void AntiAliasingManager::onResize(unsigned int sceneWidth, unsigned int sceneHeight) {
        this->sceneWidth = sceneWidth;
        this->sceneHeight = sceneHeight;

        ShaderDataSender().sendData(invSceneSizeShaderVar, Point2<float>(1.0f/(float)sceneWidth, 1.0f/(float)sceneHeight));
    }

    void AntiAliasingManager::setQuality(Quality quality) {
        this->quality = quality;

        loadFxaaShader();
    }

    void AntiAliasingManager::applyAntiAliasing() {
        if(renderer) {
            renderTarget->activeShader(fxaaShader);
            renderTarget->display(renderer);
        }
    }

}
