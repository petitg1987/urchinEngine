#include <GL/glew.h>

#include "FogManager.h"
#include "graphic/shader/ShaderManager.h"

namespace urchin {
    FogManager::FogManager() :
            deferredShaderID(0),
            hasFogLoc(0),
            fogDensityLoc(0),
            fogGradientLoc(0),
            fogColorLoc(0),
            fogMaxHeightLoc(0) {

    }

    void FogManager::pushFog(const std::shared_ptr<Fog> &fog) {
        fogs.push(fog);

        loadFog();
    }

    void FogManager::popFog() {
        if (!fogs.empty()) {
            fogs.pop();
        }

        loadFog();
    }

    std::shared_ptr<const Fog> FogManager::getCurrentFog() const {
        if (fogs.empty()) {
            return nullptr;
        }

        return fogs.top();
    }

    void FogManager::loadUniformLocationFor(unsigned int deferredShaderID) {
        this->deferredShaderID = deferredShaderID;

        ShaderManager::instance()->bind(deferredShaderID);
        hasFogLoc = glGetUniformLocation(deferredShaderID, "hasFog");
        fogDensityLoc = glGetUniformLocation(deferredShaderID, "fogDensity");
        fogGradientLoc = glGetUniformLocation(deferredShaderID, "fogGradient");
        fogColorLoc = glGetUniformLocation(deferredShaderID, "fogColor");
        fogMaxHeightLoc = glGetUniformLocation(deferredShaderID, "fogMaxHeight");

        loadFog();
    }

    void FogManager::loadFog() {
        ShaderManager::instance()->bind(deferredShaderID);
        glUniform1i(hasFogLoc, !fogs.empty());

        if (!fogs.empty()) {
            glUniform1f(fogDensityLoc, fogs.top()->getDensity());
            glUniform1f(fogGradientLoc, fogs.top()->getGradient());
            glUniform4fv(fogColorLoc, 1, (const float *)Vector4<float>(fogs.top()->getColor(), 1.0));
            glUniform1f(fogMaxHeightLoc, fogs.top()->getMaxHeight());
        }
    }

}
