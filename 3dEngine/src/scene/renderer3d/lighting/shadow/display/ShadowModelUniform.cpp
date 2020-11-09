#include <GL/glew.h>
#include <algorithm>

#include "ShadowModelUniform.h"
#include "graphic/shader/data/ShaderDataSender.h"

namespace urchin {

    ShadowModelUniform::ShadowModelUniform() :
            CustomModelUniform(),
            shadowData(nullptr) {

    }

    void ShadowModelUniform::setLayersToUpdateShaderVar(const ShaderVar &layersToUpdateShaderVar) {
        this->layersToUpdateShaderVar = layersToUpdateShaderVar;
    }

    void ShadowModelUniform::setModelUniformData(const ShadowData *shadowData) {
        this->shadowData = shadowData;
    }

    void ShadowModelUniform::loadCustomUniforms(const Model *, std::unique_ptr<Shader> &modelShader) {
        unsigned int layersToUpdate = 0;

        for (std::size_t i=0; i<shadowData->getNbFrustumShadowData(); ++i) {
            const FrustumShadowData *frustumShadowData = shadowData->getFrustumShadowData(i);
            if (frustumShadowData->needShadowMapUpdate()) {
                layersToUpdate = layersToUpdate | MathAlgorithm::powerOfTwo(i);
            }
        }

        ShaderDataSender(modelShader).sendData(layersToUpdateShaderVar, layersToUpdate);
    }

}
