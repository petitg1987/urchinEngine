#include <algorithm>

#include "ShadowModelShaderVariable.h"
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
        unsigned int layersToUpdate = 0;

        for (std::size_t i=0; i<shadowData->getNbFrustumShadowData(); ++i) {
            const FrustumShadowData* frustumShadowData = shadowData->getFrustumShadowData(i);
            if (frustumShadowData->needShadowMapUpdate()) {
                layersToUpdate = layersToUpdate | MathAlgorithm::powerOfTwo(i);
            }
        }

        ShaderDataSender().sendData(layersToUpdateShaderVar, layersToUpdate);
    }

}
