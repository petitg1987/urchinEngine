#include "ShadowShaderVariable.h"
#include "scene/renderer3d/lighting/shadow/light/LightSplitShadowMap.h"
#include "graphic/shader/data/ShaderDataSender.h"

namespace urchin {

    ShadowShaderVariable::ShadowShaderVariable() :
            CustomShaderVariable(),
            lightShadowMap(nullptr) {

    }

    void ShadowShaderVariable::setProjectionMatricesShaderVar(const ShaderVar& mModelProjectionMatrixShaderVar) {
        this->mModelProjectionMatrixShaderVar = mModelProjectionMatrixShaderVar;

        updateProjectionMatrices();
    }

    void ShadowShaderVariable::setLightShadowMap(const LightShadowMap* lightShadowMap) {
        this->lightShadowMap = lightShadowMap;

        updateProjectionMatrices();
    }

    void ShadowShaderVariable::loadCustomShaderVariables() {
        ShaderDataSender().sendData(mModelProjectionMatrixShaderVar, (unsigned int)projectionMatrices.size(), &projectionMatrices[0]);
    }

    void ShadowShaderVariable::updateProjectionMatrices() {
        projectionMatrices.clear();

        if (lightShadowMap != nullptr) {
            for(const auto& lightSplitShadowMap : lightShadowMap->getLightSplitShadowMaps()) {
                projectionMatrices.push_back(lightSplitShadowMap->getLightProjectionMatrix());
            }
        }
    }

}
