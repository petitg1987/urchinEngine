#include "ShadowShaderVariable.h"
#include "graphic/shader/data/ShaderDataSender.h"

namespace urchin {

    ShadowShaderVariable::ShadowShaderVariable() :
            CustomShaderVariable(),
            shadowData(nullptr) {

    }

    void ShadowShaderVariable::setProjectionMatricesShaderVar(const ShaderVar& mModelProjectionMatrixShaderVar) {
        this->mModelProjectionMatrixShaderVar = mModelProjectionMatrixShaderVar;

        updateProjectionMatrices();
    }

    void ShadowShaderVariable::setShadowData(const ShadowData* shadowData) {
        this->shadowData = shadowData;

        updateProjectionMatrices();
    }

    void ShadowShaderVariable::loadCustomShaderVariables() {
        ShaderDataSender().sendData(mModelProjectionMatrixShaderVar, (unsigned int)projectionMatrices.size(), &projectionMatrices[0]);
    }

    void ShadowShaderVariable::updateProjectionMatrices() {
        projectionMatrices.clear();

        if (shadowData != nullptr) {
            for (unsigned int i = 0; i < shadowData->getNbFrustumShadowData(); ++i) {
                projectionMatrices.push_back(shadowData->getFrustumShadowData(i)->getLightProjectionMatrix());
            }
        }
    }

}
