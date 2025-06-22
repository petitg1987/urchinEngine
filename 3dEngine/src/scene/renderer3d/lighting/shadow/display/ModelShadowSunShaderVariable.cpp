#include <cstring>

#include <scene/renderer3d/lighting/shadow/display/ModelShadowSunShaderVariable.h>
#include <scene/renderer3d/lighting/shadow/light/LightSplitShadowMap.h>

namespace urchin {

    ModelShadowSunShaderVariable::ModelShadowSunShaderVariable(const LightShadowMap* lightShadowMap) :
            CustomModelShaderVariable(),
            lightShadowMap(lightShadowMap),
            shadowData({}) {
        std::memset((void *)&shadowData, 0, sizeof(shadowData));
    }

    void ModelShadowSunShaderVariable::setupMeshRenderer(const std::shared_ptr<GenericRendererBuilder>& meshRendererBuilder, uint32_t uniformBinding1, uint32_t) {
        assert(meshRendererBuilder->getUniformData().size() == 2);
        std::size_t shadowDataSize = lightShadowMap->getNumberShadowMaps() * sizeof(Matrix4<float>);
        meshRendererBuilder->addUniformData(uniformBinding1, shadowDataSize, &shadowData);
    }

    void ModelShadowSunShaderVariable::loadCustomShaderVariables(GenericRenderer& meshRenderer, uint32_t uniformBinding1, uint32_t) {
        refreshShaderVariables();
        meshRenderer.updateUniformData(uniformBinding1, &shadowData);
    }

    void ModelShadowSunShaderVariable::refreshShaderVariables() {
        const std::vector<std::unique_ptr<LightSplitShadowMap>>& lightSplitShadowMaps = lightShadowMap->getLightSplitShadowMaps();
        for (unsigned int i = 0; i < lightSplitShadowMaps.size(); ++i) {
            shadowData.lightProjectionViewMatrices[i] = lightSplitShadowMaps[i]->getLightProjectionViewMatrix();
        }
    }

}
