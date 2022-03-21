#include <bitset>

#include <scene/renderer3d/lighting/shadow/display/ShadowModelShaderVariable.h>
#include <scene/renderer3d/lighting/shadow/light/LightSplitShadowMap.h>

namespace urchin {

    ShadowModelShaderVariable::ShadowModelShaderVariable(const LightShadowMap* lightShadowMap) :
            CustomModelShaderVariable(),
            lightShadowMap(lightShadowMap) {

    }

    void ShadowModelShaderVariable::setupMeshRenderer(const std::shared_ptr<GenericRendererBuilder>& meshRendererBuilder) {
        assert(meshRendererBuilder->getUniformData().size() == 2);

        if (shadowData.projectionMatrices.size() < lightShadowMap->getNumberShadowMaps()) {
            throw std::runtime_error("Number of shadow maps (" + std::to_string(lightShadowMap->getNumberShadowMaps())
                    + ") is not expected to exceed " + std::to_string(shadowData.projectionMatrices.size()));
        }
        std::size_t shadowDataSize = lightShadowMap->getNumberShadowMaps() * sizeof(Matrix4<float>);
        meshRendererBuilder->addUniformData(shadowDataSize, &shadowData); //binding 2
    }

    void ShadowModelShaderVariable::loadCustomShaderVariables(GenericRenderer& meshRenderer) {
        refreshShaderVariables();

        meshRenderer.updateUniformData(2, &shadowData);
    }

    void ShadowModelShaderVariable::refreshShaderVariables() {
        const std::vector<std::unique_ptr<LightSplitShadowMap>>& lightSplitShadowMaps = lightShadowMap->getLightSplitShadowMaps();
        for (unsigned int i = 0 ; i < lightSplitShadowMaps.size(); ++i) {
            shadowData.projectionMatrices[i] = lightSplitShadowMaps[i]->getLightProjectionMatrix();
        }
    }

}
