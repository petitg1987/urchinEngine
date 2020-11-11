#include "FogManager.h"
#include "graphic/shader/data/ShaderDataSender.h"

namespace urchin {

    void FogManager::pushFog(const std::shared_ptr<Fog> &fog) {
        fogs.push(fog);
    }

    void FogManager::popFog() {
        if (!fogs.empty()) {
            fogs.pop();
        }
    }

    std::shared_ptr<const Fog> FogManager::getActiveFog() const {
        if (fogs.empty()) {
            return nullptr;
        }
        return fogs.top();
    }

    void FogManager::initiateShaderVariables(const std::unique_ptr<Shader> &lightingShader) {
        hasFogShaderVar = ShaderVar(lightingShader, "hasFog");
        fogDensityShaderVar = ShaderVar(lightingShader, "fogDensity");
        fogGradientShaderVar = ShaderVar(lightingShader, "fogGradient");
        fogColorShaderVar = ShaderVar(lightingShader, "fogColor");
        fogMaxHeightShaderVar = ShaderVar(lightingShader, "fogMaxHeight");
    }

    void FogManager::loadFog(const std::unique_ptr<Shader> &lightingShader) {
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
