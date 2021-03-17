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

    void ShadowModelShaderVariable::setLightShadowMap(const LightShadowMap* lightShadowMap) {
        this->lightShadowMap = lightShadowMap;
    }

    void ShadowModelShaderVariable::loadCustomShaderVariables(const Model*) {
        ShaderDataSender().sendData(layersToUpdateShaderVar, (int)lightShadowMap->retrieveLayersToUpdate());
    }

}
