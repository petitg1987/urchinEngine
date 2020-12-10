#include "FrustumShadowData.h"

namespace urchin {

    FrustumShadowData::FrustumShadowData() :
            updateShadowMapThreshold(ConfigService::instance()->getFloatValue("shadow.updateShadowMapThreshold")),
            shadowCasterReceiverBoxUpdated(false),
            modelsRequireUpdate(false) {

    }

    void FrustumShadowData::updateShadowCasterReceiverBox(const AABBox<float>& shadowCasterReceiverBox, bool forceUpdateAllShadowMap) {
        if (!forceUpdateAllShadowMap && areAlmostIdenticalAABBox(shadowCasterReceiverBox, this->shadowCasterReceiverBox)) {
            this->shadowCasterReceiverBoxUpdated = false;
        } else {
            this->shadowCasterReceiverBox = shadowCasterReceiverBox;
            this->lightProjectionMatrix = shadowCasterReceiverBox.toProjectionMatrix();

            this->shadowCasterReceiverBoxUpdated = true;
        }
    }

    bool FrustumShadowData::areAlmostIdenticalAABBox(const AABBox<float>& shadowCasterReceiverBox1, const AABBox<float>& shadowCasterReceiverBox2) const {
        float updateShadowMapSquareThreshold = updateShadowMapThreshold * updateShadowMapThreshold;

        return shadowCasterReceiverBox1.getMin().squareDistance(shadowCasterReceiverBox2.getMin()) < updateShadowMapSquareThreshold
                && shadowCasterReceiverBox1.getMax().squareDistance(shadowCasterReceiverBox2.getMax()) < updateShadowMapSquareThreshold;
    }

    const Matrix4<float>& FrustumShadowData::getLightProjectionMatrix() const {
        return lightProjectionMatrix;
    }

    /**
     * @models Models visible from light in frustum split
     */
    void FrustumShadowData::updateModels(const std::vector<Model*>& models) {
        modelsRequireUpdate = false;

        if (models != this->models) {
            modelsRequireUpdate = true;
        } else {
            for (auto* model : models) {
                if (model->isMovingInOctree() || model->isAnimate()) {
                    modelsRequireUpdate = true;
                    break;
                }
            }
        }

        this->models = models;
    }

    /**
     * @return Models visible from light in frustum split
     */
    const std::vector<Model*>& FrustumShadowData::getModels() const {
        return models;
    }

    bool FrustumShadowData::needShadowMapUpdate() const {
        return shadowCasterReceiverBoxUpdated || modelsRequireUpdate;
    }
}
