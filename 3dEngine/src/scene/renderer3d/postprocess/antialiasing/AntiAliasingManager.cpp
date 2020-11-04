#include <GL/glew.h>
#include <map>
#include <string>

#include "scene/renderer3d/postprocess/antialiasing/AntiAliasingManager.h"
#include "graphic/shader/ShaderManager.h"
#include "graphic/displayer/generic/GenericDisplayerBuilder.h"

#define DEFAULT_AA_QUALITY AntiAliasingManager::Quality::VERY_HIGH

namespace urchin {

    AntiAliasingManager::AntiAliasingManager() :
        quality(DEFAULT_AA_QUALITY),
        sceneWidth(0),
        sceneHeight(0),
        fxaaShader(0),
        texLoc(0),
        invSceneSizeLoc(0) {
        loadFxaaShader();

        displayer = std::make_unique<GenericDisplayerBuilder>(ShapeType::RECTANGLE)->build();
    }

    AntiAliasingManager::~AntiAliasingManager() {
        ShaderManager::instance()->removeProgram(fxaaShader);
    }

    void AntiAliasingManager::loadFxaaShader() {
        std::map<std::string, std::string> fxaaTokens;
        fxaaTokens["FXAA_QUALITY"] = std::to_string(static_cast<int>(quality));

        ShaderManager::instance()->removeProgram(fxaaShader);
        fxaaShader = ShaderManager::instance()->createProgram("fxaa.vert", "", "fxaa.frag", fxaaTokens);

        ShaderManager::instance()->bind(fxaaShader);
        texLoc = glGetUniformLocation(fxaaShader, "tex");
        glUniform1i(texLoc, 0);
        invSceneSizeLoc = glGetUniformLocation(fxaaShader, "invSceneSize");
        glUniform2f(invSceneSizeLoc, 1.0f/(float)sceneWidth, 1.0f/(float)sceneHeight);
    }

    void AntiAliasingManager::onResize(unsigned int sceneWidth, unsigned int sceneHeight) {
        ShaderManager::instance()->bind(fxaaShader);

        this->sceneWidth = sceneWidth;
        this->sceneHeight = sceneHeight;

        glUniform2f(invSceneSizeLoc, 1.0f/(float)sceneWidth, 1.0f/(float)sceneHeight);
    }

    void AntiAliasingManager::setQuality(Quality quality) {
        this->quality = quality;

        loadFxaaShader();
    }

    void AntiAliasingManager::applyOn(unsigned int textureId) {
        ShaderManager::instance()->bind(fxaaShader);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureId);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        displayer->display();
    }

}
