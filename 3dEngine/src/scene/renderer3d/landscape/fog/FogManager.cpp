#include "FogManager.h"
#include "graphic/shader/data/ShaderDataSender.h"

namespace urchin {

    void FogManager::pushFog(const std::shared_ptr<Fog> &fog) { //TODO find better way to manage fog
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

    void FogManager::initiateShaderVariables(const std::shared_ptr<Shader> &lightingShader) {
        this->lightingShader = lightingShader;

        hasFogShaderVar = ShaderVar(lightingShader, "hasFog");
        fogDensityShaderVar = ShaderVar(lightingShader, "fogDensity");
        fogGradientShaderVar = ShaderVar(lightingShader, "fogGradient");
        fogColorShaderVar = ShaderVar(lightingShader, "fogColor");
        fogMaxHeightShaderVar = ShaderVar(lightingShader, "fogMaxHeight");

        loadFog();
    }

    void FogManager::loadFog() {
        ShaderDataSender(lightingShader).sendData(hasFogShaderVar, !fogs.empty());

        if (!fogs.empty()) {
            ShaderDataSender(lightingShader)
                .sendData(fogDensityShaderVar, fogs.top()->getDensity())
                .sendData(fogGradientShaderVar, fogs.top()->getGradient())
                .sendData(fogColorShaderVar, Vector4<float>(fogs.top()->getColor(), 1.0))
                .sendData(fogMaxHeightShaderVar, fogs.top()->getMaxHeight());
        }
    }

}
