#include "ShadowUniform.h"
#include "graphic/shader/data/ShaderDataSender.h"

namespace urchin {

    ShadowUniform::ShadowUniform() :
            CustomUniform(),
            shadowData(nullptr) {

    }

    void ShadowUniform::setProjectionMatricesShaderVar(const ShaderVar &mModelProjectionMatrixShaderVar) {
        this->mModelProjectionMatrixShaderVar = mModelProjectionMatrixShaderVar;

        updateProjectionMatrices();
    }

    void ShadowUniform::setUniformData(const ShadowData *shadowData) {
        this->shadowData = shadowData;

        updateProjectionMatrices();
    }

    void ShadowUniform::loadCustomUniforms(std::unique_ptr<Shader> &modelShader) { //TODO rename all uniforms methods name & class name
        ShaderDataSender(modelShader).sendData(mModelProjectionMatrixShaderVar, projectionMatrices[0]);
    }

    void ShadowUniform::updateProjectionMatrices() {
        projectionMatrices.clear();

        if (shadowData!=nullptr) {
            for (std::size_t i = 0; i < shadowData->getNbFrustumShadowData(); ++i) {
                projectionMatrices.push_back(shadowData->getFrustumShadowData(i)->getLightProjectionMatrix());
            }
        }
    }

}
