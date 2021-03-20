#include "ShadowModelShaderVariable.h"
#include "scene/renderer3d/lighting/shadow/light/LightSplitShadowMap.h"
#include "graphic/shader/data/ShaderDataSender.h"

namespace urchin {

    ShadowModelShaderVariable::ShadowModelShaderVariable() :
            CustomModelShaderVariable(),
            lightShadowMap(nullptr) {

    }

    void ShadowModelShaderVariable::setLayersToUpdateShaderVar(const ShaderVar& layersToUpdateShaderVar) {
        this->layersToUpdateShaderVar = layersToUpdateShaderVar;
    }

    void ShadowModelShaderVariable::setProjectionMatricesShaderVar(const ShaderVar& mModelProjectionMatrixShaderVar) {
        this->mModelProjectionMatrixShaderVar = mModelProjectionMatrixShaderVar;
    }

    void ShadowModelShaderVariable::setLightShadowMap(const LightShadowMap* lightShadowMap) {
        this->lightShadowMap = lightShadowMap;

        updateProjectionMatrices();
    }

    void ShadowModelShaderVariable::loadCustomShaderVariables(const Model*) {
        ShaderDataSender().sendData(layersToUpdateShaderVar, (int)lightShadowMap->retrieveLayersToUpdate());
        ShaderDataSender().sendData(mModelProjectionMatrixShaderVar, (unsigned int)projectionMatrices.size(), &projectionMatrices[0]);
    }

    void ShadowModelShaderVariable::updateProjectionMatrices() {
        projectionMatrices.clear();

        if (lightShadowMap != nullptr) {
            for (const auto& lightSplitShadowMap : lightShadowMap->getLightSplitShadowMaps()) {
                projectionMatrices.push_back(lightSplitShadowMap->getLightProjectionMatrix());
            }
        }
    }

}
