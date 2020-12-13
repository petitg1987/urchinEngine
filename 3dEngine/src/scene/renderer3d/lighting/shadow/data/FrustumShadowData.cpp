#include "FrustumShadowData.h"
#include "scene/renderer3d/lighting/shadow/data/ShadowData.h"
#include "scene/renderer3d/lighting/shadow/filter/ModelProduceShadowFilter.h"

namespace urchin {

    FrustumShadowData::FrustumShadowData(ShadowData* shadowData) :
            shadowData(shadowData),
            updateShadowMapThreshold(ConfigService::instance()->getFloatValue("shadow.updateShadowMapThreshold")),
            shadowCasterReceiverBoxUpdated(false),
            modelsRequireUpdate(false) {

    }

    void FrustumShadowData::update(const Frustum<float>&splitFrustums, bool bForceUpdateAllShadowMaps) {
        AABBox<float> aabboxSceneIndependent = createSceneIndependentBox(splitFrustums, shadowData->getLightViewMatrix());
        OBBox<float> obboxSceneIndependentViewSpace = shadowData->getLightViewMatrix().inverse() * OBBox<float>(aabboxSceneIndependent);

        obboxModels.clear();
        shadowData->getModelOctreeManager()->getOctreeablesIn(obboxSceneIndependentViewSpace, obboxModels, ModelProduceShadowFilter());
        updateModels(obboxModels);
        buildSceneDependentBox(aabboxSceneIndependent, obboxSceneIndependentViewSpace, bForceUpdateAllShadowMaps);
    }

    const AABBox<float> &FrustumShadowData::getShadowCasterReceiverBox() const {
        return shadowCasterReceiverBox;
    }

    const Matrix4<float>& FrustumShadowData::getLightProjectionMatrix() const {
        return lightProjectionMatrix;
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

    /**
     * @return Box in light space containing shadow caster and receiver (scene independent)
     */
    AABBox<float> FrustumShadowData::createSceneIndependentBox(const Frustum<float>& splitFrustum, const Matrix4<float>& lightViewMatrix) const {
        ScopeProfiler sp(Profiler::graphic(), "sceneIndepBox");

        const Frustum<float>& frustumLightSpace = lightViewMatrix * splitFrustum;

        //determine point belonging to shadow caster/receiver box
        Point3<float> shadowReceiverAndCasterVertex[16];
        float nearCapZ = computeNearZForSceneIndependentBox(frustumLightSpace);
        for (unsigned int i = 0; i < 8; ++i) {
            const Point3<float>& frustumPoint = frustumLightSpace.getFrustumPoints()[i];

            //add shadow receiver points
            shadowReceiverAndCasterVertex[i * 2] = frustumPoint;

            //add shadow caster points
            shadowReceiverAndCasterVertex[i * 2 + 1] = Point3<float>(frustumPoint.X, frustumPoint.Y, nearCapZ);
        }

        //build shadow receiver/caster bounding box from points
        return AABBox<float>(shadowReceiverAndCasterVertex, 16);
    }

    float FrustumShadowData::computeNearZForSceneIndependentBox(const Frustum<float>& splitFrustumLightSpace) const {
        float nearestPointFromLight = splitFrustumLightSpace.getFrustumPoints()[0].Z;
        for (unsigned int i = 1; i < 8; ++i) {
            if (splitFrustumLightSpace.getFrustumPoints()[i].Z > nearestPointFromLight) {
                nearestPointFromLight = splitFrustumLightSpace.getFrustumPoints()[i].Z;
            }
        }
        return nearestPointFromLight + shadowData->getViewingShadowDistance();
    }

    /**
     * Build box in light space containing shadow caster and receiver (scene dependent)
     */
    void FrustumShadowData::buildSceneDependentBox(const AABBox<float>& aabboxSceneIndependent, const OBBox<float>& obboxSceneIndependentViewSpace, bool forceUpdateAllShadowMap) {
        ScopeProfiler sp(Profiler::graphic(), "sceneDepBox");

        AABBox<float> aabboxSceneDependent;
        if (!models.empty()) {
            aabboxSceneDependent = AABBox<float>::initMergeableAABBox();

            for (const auto& model : models) {
                if (model->getSplitAABBoxes().size() == 1) {
                    aabboxSceneDependent = aabboxSceneDependent.merge(shadowData->getLightViewMatrix() * model->getSplitAABBoxes()[0]);
                } else {
                    for (const auto& splitAABBox : model->getSplitAABBoxes()) {
                        if (obboxSceneIndependentViewSpace.collideWithAABBox(splitAABBox)) {
                            aabboxSceneDependent = aabboxSceneDependent.merge(shadowData->getLightViewMatrix() * splitAABBox);
                        }
                    }
                }
            }
        }

        Point3<float> cutMin(
                std::min(std::max(aabboxSceneDependent.getMin().X, aabboxSceneIndependent.getMin().X), aabboxSceneIndependent.getMax().X),
                std::min(std::max(aabboxSceneDependent.getMin().Y, aabboxSceneIndependent.getMin().Y), aabboxSceneIndependent.getMax().Y),
                aabboxSceneIndependent.getMin().Z); //shadow can be projected outside the box: value cannot be capped

        Point3<float> cutMax(
                std::max(std::min(aabboxSceneDependent.getMax().X, aabboxSceneIndependent.getMax().X), aabboxSceneIndependent.getMin().X),
                std::max(std::min(aabboxSceneDependent.getMax().Y, aabboxSceneIndependent.getMax().Y), aabboxSceneIndependent.getMin().Y),
                std::max(std::min(aabboxSceneDependent.getMax().Z, aabboxSceneIndependent.getMax().Z), aabboxSceneIndependent.getMin().Z));

        //avoid AABBox of zero size because cause some shadow artifacts
        cutMin -= 0.0001f;
        cutMax += 0.0001f;

        AABBox<float> adjustedAabboxSceneDependent = AABBox<float>(cutMin, cutMax);
        updateShadowCasterReceiverBox(adjustedAabboxSceneDependent, forceUpdateAllShadowMap);
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
}
