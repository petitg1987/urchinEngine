#include <GL/glew.h>

#include "ShadowUniform.h"

namespace urchin {

    ShadowUniform::ShadowUniform() :
            CustomUniform(),
            mModelProjectionMatrixLoc(0),
            shadowData(nullptr) {

    }

    void ShadowUniform::setProjectionMatricesLocation(int mModelProjectionMatrixLoc) {
        this->mModelProjectionMatrixLoc = mModelProjectionMatrixLoc;

        updateProjectionMatrices();
    }

    void ShadowUniform::setUniformData(const ShadowData *shadowData) {
        this->shadowData = shadowData;

        updateProjectionMatrices();
    }

    void ShadowUniform::loadCustomUniforms() {
        glUniformMatrix4fv(mModelProjectionMatrixLoc, projectionMatrices.size(), GL_FALSE, (const float*)projectionMatrices[0]);
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
