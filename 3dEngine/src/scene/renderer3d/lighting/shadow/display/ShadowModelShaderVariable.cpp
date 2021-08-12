#include <scene/renderer3d/lighting/shadow/display/ShadowModelShaderVariable.h>
#include <scene/renderer3d/lighting/shadow/light/LightSplitShadowMap.h>

namespace urchin {

    ShadowModelShaderVariable::ShadowModelShaderVariable(const LightShadowMap* lightShadowMap) :
            CustomModelShaderVariable(),
            lightShadowMap(lightShadowMap) {
        shadowData.layerToUpdate = -1;
    }

    void ShadowModelShaderVariable::setupMeshRenderer(const std::shared_ptr<GenericRendererBuilder>& meshRendererBuilder) {
        shadowData.layerToUpdate = (int)lightShadowMap->retrieveLayersToUpdate();

        if (shadowData.projectionMatrices.size() < lightShadowMap->getNumberShadowMaps()) {
            throw std::runtime_error("Number of shadow maps (" + std::to_string(lightShadowMap->getNumberShadowMaps())
                    + ") is not expected to exceed " + std::to_string(shadowData.projectionMatrices.size()));
        }
        std::size_t shadowDataSize = sizeof(shadowData.layerToUpdate) + lightShadowMap->getNumberShadowMaps() * sizeof(Matrix4<float>);
        meshRendererBuilder
                ->addUniformData(shadowDataSize, &shadowData); //binding 3
    }

    void ShadowModelShaderVariable::loadCustomShaderVariables(GenericRenderer& meshRenderer) {
        refreshShaderVariables();

        meshRenderer.updateUniformData(3, &shadowData);
    }

    void ShadowModelShaderVariable::refreshShaderVariables() {
        shadowData.layerToUpdate = (int)lightShadowMap->retrieveLayersToUpdate();

        const std::vector<std::unique_ptr<LightSplitShadowMap>>& lightSplitShadowMaps = lightShadowMap->getLightSplitShadowMaps();
        for (unsigned int i = 0 ; i < lightSplitShadowMaps.size(); ++i) {
            shadowData.projectionMatrices[i] = lightSplitShadowMaps[i]->getLightProjectionMatrix();
        }
    }

}
