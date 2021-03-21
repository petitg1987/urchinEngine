#include "ShadowModelShaderVariable.h"
#include "scene/renderer3d/lighting/shadow/light/LightSplitShadowMap.h"
#include "graphic/shader/data/ShaderDataSender.h"

namespace urchin {

    ShadowModelShaderVariable::ShadowModelShaderVariable(const LightShadowMap* lightShadowMap, const ShaderVar& mModelProjectionMatrixShaderVar, const ShaderVar& layersToUpdateShaderVar) :
            CustomModelShaderVariable(),
            mModelProjectionMatrixShaderVar(mModelProjectionMatrixShaderVar),
            layersToUpdateShaderVar(layersToUpdateShaderVar),
            lightShadowMap(lightShadowMap) {

    }

    void ShadowModelShaderVariable::setupMeshRenderer(const std::unique_ptr<GenericRendererBuilder>& meshRendererBuilder) {
        meshRendererBuilder->addShaderData(ShaderDataSender()
                .sendData(mModelProjectionMatrixShaderVar, (unsigned int)projectionMatrices.size(), &projectionMatrices[0])
                .sendData(layersToUpdateShaderVar, (int)lightShadowMap->retrieveLayersToUpdate())); //binding 2
    }

    void ShadowModelShaderVariable::loadCustomShaderVariables(const std::unique_ptr<GenericRenderer>& meshRenderer) { //TODO use struct
        updateProjectionMatrices();

        meshRenderer->updateShaderData(2, ShaderDataSender()
                .sendData(mModelProjectionMatrixShaderVar, (unsigned int)projectionMatrices.size(), &projectionMatrices[0])
                .sendData(layersToUpdateShaderVar, (int)lightShadowMap->retrieveLayersToUpdate()));
    }

    void ShadowModelShaderVariable::updateProjectionMatrices() {
        projectionMatrices.clear();
        for (const auto& lightSplitShadowMap : lightShadowMap->getLightSplitShadowMaps()) {
            projectionMatrices.push_back(lightSplitShadowMap->getLightProjectionMatrix());
        }
    }

}
