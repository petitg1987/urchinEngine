#include "ShadowModelShaderVariable.h"
#include "scene/renderer3d/lighting/shadow/data/FrustumShadowData.h"
#include "graphic/shader/data/ShaderDataSender.h"

namespace urchin {

    ShadowModelShaderVariable::ShadowModelShaderVariable() :
            CustomModelShaderVariable(),
            shadowData(nullptr) {

    }

    void ShadowModelShaderVariable::setLayersToUpdateShaderVar(const ShaderVar& layersToUpdateShaderVar) {
        this->layersToUpdateShaderVar = layersToUpdateShaderVar;
    }

    void ShadowModelShaderVariable::setShadowData(const ShadowData* shadowData) {
        this->shadowData = shadowData;
    }

    void ShadowModelShaderVariable::loadCustomShaderVariables(const Model*) {
        ShaderDataSender().sendData(layersToUpdateShaderVar, shadowData->retrieveLayersToUpdate());
    }

}
