#include <GL/glew.h>

#include "FogManager.h"
#include "graphic/shader/builder/ShaderBuilder.h"

namespace urchin {
    FogManager::FogManager() :
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

    void FogManager::loadUniformLocationFor(const std::shared_ptr<Shader> &deferredShader) {
        this->deferredShader = deferredShader;

        deferredShader->bind();
        hasFogLoc = glGetUniformLocation(deferredShader->getShaderId(), "hasFog");
        fogDensityLoc = glGetUniformLocation(deferredShader->getShaderId(), "fogDensity");
        fogGradientLoc = glGetUniformLocation(deferredShader->getShaderId(), "fogGradient");
        fogColorLoc = glGetUniformLocation(deferredShader->getShaderId(), "fogColor");
        fogMaxHeightLoc = glGetUniformLocation(deferredShader->getShaderId(), "fogMaxHeight");

        loadFog();
    }

    void FogManager::loadFog() {
        deferredShader->bind();
        glUniform1i(hasFogLoc, !fogs.empty());

        if (!fogs.empty()) {
            glUniform1f(fogDensityLoc, fogs.top()->getDensity());
            glUniform1f(fogGradientLoc, fogs.top()->getGradient());
            glUniform4fv(fogColorLoc, 1, (const float *)Vector4<float>(fogs.top()->getColor(), 1.0));
            glUniform1f(fogMaxHeightLoc, fogs.top()->getMaxHeight());
        }
    }

}
