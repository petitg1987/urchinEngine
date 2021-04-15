#include "ShadowModelShaderVariable.h"
#include "scene/renderer3d/lighting/shadow/light/LightSplitShadowMap.h"

namespace urchin {

    ShadowModelShaderVariable::ShadowModelShaderVariable(const LightShadowMap* lightShadowMap) :
            CustomModelShaderVariable(),
            lightShadowMap(lightShadowMap),
            layerToUpdate(-1) {

    }

    void ShadowModelShaderVariable::setupMeshRenderer(const std::shared_ptr<GenericRendererBuilder>& meshRendererBuilder) {
        projectionMatrices.resize(lightShadowMap->getNumberShadowMaps());
        layerToUpdate = (int)lightShadowMap->retrieveLayersToUpdate();

        meshRendererBuilder
                ->addUniformData(lightShadowMap->getNumberShadowMaps() * sizeof(Matrix4<float>), &projectionMatrices[0]) //binding 2
                ->addUniformData(sizeof(layerToUpdate), &layerToUpdate); //binding 3
    }

    void ShadowModelShaderVariable::loadCustomShaderVariables(const std::shared_ptr<GenericRenderer>& meshRenderer) {
        refreshShaderVariables();

        meshRenderer->updateUniformData(2, &projectionMatrices[0]);
        meshRenderer->updateUniformData(3, &layerToUpdate);
    }

    void ShadowModelShaderVariable::refreshShaderVariables() {
        projectionMatrices.clear();
        for (const auto& lightSplitShadowMap : lightShadowMap->getLightSplitShadowMaps()) {
            projectionMatrices.push_back(lightSplitShadowMap->getLightProjectionMatrix());
        }

        layerToUpdate = (int)lightShadowMap->retrieveLayersToUpdate();
    }

}
