#include <cstring>

#include <scene/renderer3d/lighting/shadow/display/ModelShadowOmnidirectionalShaderVariable.h>
#include <scene/renderer3d/lighting/shadow/light/LightSplitShadowMap.h>

namespace urchin {

    ModelShadowOmnidirectionalShaderVariable::ModelShadowOmnidirectionalShaderVariable(const LightShadowMap* lightShadowMap) :
            CustomModelShaderVariable(),
            lightShadowMap(lightShadowMap),
            shadowMatrixData({}),
            shadowScopeData({}) {
        std::memset((void *)&shadowMatrixData, 0, sizeof(shadowMatrixData));
        std::memset((void *)&shadowScopeData, 0, sizeof(shadowScopeData));
    }

    void ModelShadowOmnidirectionalShaderVariable::setupMeshRenderer(const std::shared_ptr<GenericRendererBuilder>& meshRendererBuilder, uint32_t uniformBinding1, uint32_t uniformBinding2) {
        assert(meshRendererBuilder->getUniformData().size() == 2);
        std::size_t shadowDataSize = lightShadowMap->getNumberShadowMaps() * sizeof(Matrix4<float>);
        meshRendererBuilder->addUniformData(uniformBinding1, shadowDataSize, &shadowMatrixData);

        meshRendererBuilder->addUniformData(uniformBinding2, sizeof(shadowScopeData), &shadowScopeData);
    }

    void ModelShadowOmnidirectionalShaderVariable::loadCustomShaderVariables(GenericRenderer& meshRenderer, uint32_t uniformBinding1, uint32_t uniformBinding2) {
        refreshShaderVariables();
        meshRenderer.updateUniformData(uniformBinding1, &shadowMatrixData);
        meshRenderer.updateUniformData(uniformBinding2, &shadowScopeData);
    }

    void ModelShadowOmnidirectionalShaderVariable::refreshShaderVariables() {
        const std::vector<std::unique_ptr<LightSplitShadowMap>>& lightSplitShadowMaps = lightShadowMap->getLightSplitShadowMaps();
        for (unsigned int i = 0; i < lightSplitShadowMaps.size(); ++i) {
            shadowMatrixData.lightProjectionViewMatrices[i] = lightSplitShadowMaps[i]->getLightProjectionViewMatrix();
        }

        shadowScopeData.omnidirectionalNearPlane = lightSplitShadowMaps[0]->getNearPlane();
        shadowScopeData.omnidirectionalFarPlane = lightSplitShadowMaps[0]->getFarPlane();
    }

}
