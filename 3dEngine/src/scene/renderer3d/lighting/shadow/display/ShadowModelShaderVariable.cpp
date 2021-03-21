#include "ShadowModelShaderVariable.h"
#include "scene/renderer3d/lighting/shadow/light/LightSplitShadowMap.h"
#include "graphic/shader/data/ShaderDataSender.h"

namespace urchin {

    ShadowModelShaderVariable::ShadowModelShaderVariable(const LightShadowMap* lightShadowMap, const ShaderVar& mModelProjectionMatrixShaderVar, const ShaderVar& layersToUpdateShaderVar) :
            CustomModelShaderVariable(),
            mModelProjectionMatrixShaderVar(mModelProjectionMatrixShaderVar),
            layersToUpdateShaderVar(layersToUpdateShaderVar),
            lightShadowMap(lightShadowMap),
            layerToUpdate(-1) {

    }

    void ShadowModelShaderVariable::setupMeshRenderer(const std::unique_ptr<GenericRendererBuilder>& meshRendererBuilder) {
        projectionMatrices.resize(lightShadowMap->getNumberShadowMaps());

        meshRendererBuilder
                //Vulkan source code: ->addShaderData(lightShadowMap->getNumberShadowMaps() * sizeof(Matrix4<float>), &projectionMatrices[0])
                ->addShaderData(ShaderDataSender().sendData(mModelProjectionMatrixShaderVar, (unsigned int)projectionMatrices.size(), &projectionMatrices[0])) //binding 2
                ->addShaderData(ShaderDataSender().sendData(layersToUpdateShaderVar, (int)lightShadowMap->retrieveLayersToUpdate())); //binding 3
    }

    void ShadowModelShaderVariable::loadCustomShaderVariables(const std::unique_ptr<GenericRenderer>& meshRenderer) {
        refreshShaderVariables();

        meshRenderer->updateShaderData(2, ShaderDataSender().sendData(mModelProjectionMatrixShaderVar, (unsigned int)projectionMatrices.size(), &projectionMatrices[0]));
        meshRenderer->updateShaderData(3, ShaderDataSender().sendData(layersToUpdateShaderVar, layerToUpdate));
    }

    void ShadowModelShaderVariable::refreshShaderVariables() {
        projectionMatrices.clear();
        for (const auto& lightSplitShadowMap : lightShadowMap->getLightSplitShadowMaps()) {
            projectionMatrices.push_back(lightSplitShadowMap->getLightProjectionMatrix());
        }

        layerToUpdate = (int)lightShadowMap->retrieveLayersToUpdate();
    }

}
