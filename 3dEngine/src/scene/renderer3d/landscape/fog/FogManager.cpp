#include "FogManager.h"
#include "graphic/shader/data/ShaderDataSender.h"

namespace urchin {

    FogManager::FogManager() :
        fogData({}) {

    }

    void FogManager::pushFog(const std::shared_ptr<Fog>& fog) {
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

    void FogManager::initiateShaderVariables(const std::shared_ptr<Shader>& lightingShader) {
        hasFogShaderVar = ShaderVar(lightingShader, "hasFog");
        fogDensityShaderVar = ShaderVar(lightingShader, "fogDensity");
        fogGradientShaderVar = ShaderVar(lightingShader, "fogGradient");
        fogColorShaderVar = ShaderVar(lightingShader, "fogColor");
        fogMaxHeightShaderVar = ShaderVar(lightingShader, "fogMaxHeight");
    }

    void FogManager::setupLightingRenderer(const std::unique_ptr<GenericRendererBuilder>& lightingRendererBuilder) {
        lightingRendererBuilder->addShaderData(ShaderDataSender()
                .sendData(hasFogShaderVar, fogData.hasFog)
                .sendData(fogDensityShaderVar, fogData.density)
                .sendData(fogGradientShaderVar, fogData.gradient)
                .sendData(fogMaxHeightShaderVar, fogData.maxHeight)
                .sendData(fogColorShaderVar, fogData.color)); //binding 6
    }

    void FogManager::loadFog(const std::unique_ptr<GenericRenderer>& lightingRenderer) {
        fogData = {};
        fogData.hasFog = !fogs.empty();
        if (!fogs.empty()) {
            fogData.density = fogs.top()->getDensity();
            fogData.gradient = fogs.top()->getGradient();
            fogData.maxHeight = fogs.top()->getMaxHeight();
            fogData.color = Vector4<float>(fogs.top()->getColor(), 1.0);
        }

        lightingRenderer->updateShaderData(6, ShaderDataSender()
                .sendData(hasFogShaderVar, fogData.hasFog)
                .sendData(fogDensityShaderVar, fogData.density)
                .sendData(fogGradientShaderVar, fogData.gradient)
                .sendData(fogMaxHeightShaderVar, fogData.maxHeight)
                .sendData(fogColorShaderVar, fogData.color));
    }

}
