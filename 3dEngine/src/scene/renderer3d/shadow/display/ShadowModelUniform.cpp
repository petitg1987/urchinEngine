#include <GL/glew.h>
#include <algorithm>

#include "ShadowModelUniform.h"

namespace urchin {

    ShadowModelUniform::ShadowModelUniform() :
            CustomModelUniform(),
            layersToUpdateLoc(0),
            shadowData(nullptr) {

    }

    void ShadowModelUniform::setLayersToUpdateLocation(int layersToUpdateLoc) {
        this->layersToUpdateLoc = layersToUpdateLoc;
    }

    void ShadowModelUniform::setModelUniformData(const ShadowData *shadowData) {
        this->shadowData = shadowData;
    }

    void ShadowModelUniform::loadCustomUniforms(const Model *) {
        unsigned int layersToUpdate = 0;

        for (std::size_t i=0; i<shadowData->getNbFrustumShadowData(); ++i) {
            const FrustumShadowData *frustumShadowData = shadowData->getFrustumShadowData(i);
            if (frustumShadowData->needShadowMapUpdate()) {
                layersToUpdate = layersToUpdate | MathAlgorithm::powerOfTwo(i);
            }
        }

        glUniform1ui(layersToUpdateLoc, layersToUpdate);
    }

}
