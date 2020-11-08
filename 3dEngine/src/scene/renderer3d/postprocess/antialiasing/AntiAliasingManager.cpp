#include <map>
#include <string>

#include "scene/renderer3d/postprocess/antialiasing/AntiAliasingManager.h"
#include "graphic/shader/builder/ShaderBuilder.h"
#include "graphic/shader/data/ShaderDataSender.h"
#include "graphic/render/GenericRendererBuilder.h"

#define DEFAULT_AA_QUALITY AntiAliasingManager::Quality::VERY_HIGH

namespace urchin {

    AntiAliasingManager::AntiAliasingManager(unsigned int textureId) :
            quality(DEFAULT_AA_QUALITY),
            sceneWidth(0),
            sceneHeight(0) {
        loadFxaaShader();

        std::vector<int> vertexCoord = {-1, 1, 1, 1, 1, -1, -1, -1};
        std::vector<int> textureCoord = {0, 1, 1, 1, 1, 0, 0, 0};
        renderer = std::make_unique<GenericRendererBuilder>(ShapeType::RECTANGLE)
                ->vertexData(CoordType::INT, CoordDimension::_2D, &vertexCoord[0])
                ->textureData(CoordType::INT, CoordDimension::_2D, &textureCoord[0])
                ->addTexture(Texture::build(textureId, Texture::DEFAULT, TextureParam::buildLinear()))
                ->build();
    }

    void AntiAliasingManager::loadFxaaShader() {
        std::map<std::string, std::string> fxaaTokens;
        fxaaTokens["FXAA_QUALITY"] = std::to_string(static_cast<int>(quality));

        fxaaShader = ShaderBuilder().createShader("fxaa.vert", "", "fxaa.frag", fxaaTokens);
        invSceneSizeShaderVar = ShaderVar(fxaaShader, "invSceneSize");

        ShaderDataSender(fxaaShader)
                .sendData(ShaderVar(fxaaShader, "tex"), 0)
                .sendData(invSceneSizeShaderVar, Point2<float>(1.0f/(float)sceneWidth, 1.0f/(float)sceneHeight));
    }

    void AntiAliasingManager::onResize(unsigned int sceneWidth, unsigned int sceneHeight) {
        this->sceneWidth = sceneWidth;
        this->sceneHeight = sceneHeight;

        ShaderDataSender(fxaaShader).sendData(invSceneSizeShaderVar, Point2<float>(1.0f/(float)sceneWidth, 1.0f/(float)sceneHeight));
    }

    void AntiAliasingManager::setQuality(Quality quality) {
        this->quality = quality;

        loadFxaaShader();
    }

    void AntiAliasingManager::applyAntiAliasing() {
        fxaaShader->bind();
        renderer->draw();
    }

}
