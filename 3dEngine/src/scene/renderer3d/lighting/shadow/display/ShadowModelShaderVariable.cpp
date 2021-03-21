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

    void ShadowModelShaderVariable::setupMeshRenderer(const std::unique_ptr<GenericRendererBuilder>& meshRendererBuilder) {
        meshRendererBuilder->addShaderData(ShaderDataSender()
                .sendData(mModelProjectionMatrixShaderVar, (unsigned int)projectionMatrices.size(), &projectionMatrices[0])
                .sendData(layersToUpdateShaderVar, (int)lightShadowMap->retrieveLayersToUpdate())); //binding 2
    }

    void ShadowModelShaderVariable::loadCustomShaderVariables(const std::unique_ptr<GenericRenderer>& meshRenderer) { //TODO use struct
        meshRenderer->updateShaderData(2, ShaderDataSender()
                .sendData(mModelProjectionMatrixShaderVar, (unsigned int)projectionMatrices.size(), &projectionMatrices[0])
                .sendData(layersToUpdateShaderVar, (int)lightShadowMap->retrieveLayersToUpdate()));
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
