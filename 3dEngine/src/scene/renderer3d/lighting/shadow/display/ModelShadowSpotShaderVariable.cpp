#include <cstring>

#include "scene/renderer3d/lighting/shadow/display/ModelShadowSpotShaderVariable.h"
#include "scene/renderer3d/lighting/shadow/light/LightSplitShadowMap.h"

namespace urchin {

    ModelShadowSpotShaderVariable::ModelShadowSpotShaderVariable(const LightShadowMap* lightShadowMap) :
            CustomModelShaderVariable(),
            lightShadowMap(lightShadowMap),
            shadowData({}) {
        std::memset((void*)&shadowData, 0, sizeof(shadowData));
    }

    void ModelShadowSpotShaderVariable::setupMeshRenderer(const std::shared_ptr<GenericRendererBuilder>& meshRendererBuilder, uint32_t uniformBinding1, uint32_t) {
        meshRendererBuilder->addUniformData(uniformBinding1, sizeof(shadowData), &shadowData);
    }

    void ModelShadowSpotShaderVariable::loadCustomShaderVariables(GenericRenderer& meshRenderer, uint32_t uniformBinding1, uint32_t) {
        refreshShaderVariables();
        meshRenderer.updateUniformData(uniformBinding1, &shadowData);
    }

    void ModelShadowSpotShaderVariable::refreshShaderVariables() {
        const std::vector<std::unique_ptr<LightSplitShadowMap>>& lightSplitShadowMaps = lightShadowMap->getLightSplitShadowMaps();
        assert(lightSplitShadowMaps.size() == 1);
        shadowData.lightProjectionViewMatrix = lightSplitShadowMaps[0]->getLightProjectionViewMatrix();
    }

}
