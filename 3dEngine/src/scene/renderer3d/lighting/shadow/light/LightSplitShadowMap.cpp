#include <scene/renderer3d/lighting/shadow/light/LightSplitShadowMap.h>
#include <scene/renderer3d/lighting/shadow/light/LightShadowMap.h>
#include <scene/renderer3d/lighting/shadow/filter/ModelProduceShadowFilter.h>

namespace urchin {

    //static
    const float LightSplitShadowMap::LIGHT_BOX_MARGIN = 0.001f;

    LightSplitShadowMap::LightSplitShadowMap(LightShadowMap* lightShadowMap) :
            lightShadowMap(lightShadowMap),
            updateShadowMapThreshold(ConfigService::instance().getFloatValue("shadow.updateShadowMapThreshold")),
            shadowCasterReceiverBoxUpdated(false),
            modelsRequireUpdate(false) {

    }

    void LightSplitShadowMap::update(const Frustum<float>& splitFrustums, bool bForceUpdateAllShadowMaps) {
        AABBox<float> aabboxSceneIndependent = createSceneIndependentBox(splitFrustums, lightShadowMap->getLightViewMatrix());
        OBBox<float> obboxSceneIndependentViewSpace = lightShadowMap->getLightViewMatrix().inverse() * OBBox<float>(aabboxSceneIndependent);

        obboxModels.clear();
        lightShadowMap->getModelOctreeManager().getOctreeablesIn(obboxSceneIndependentViewSpace, obboxModels, ModelProduceShadowFilter());
        updateModels(obboxModels);
        buildSceneDependentBox(aabboxSceneIndependent, obboxSceneIndependentViewSpace, bForceUpdateAllShadowMaps);
    }

    const AABBox<float> &LightSplitShadowMap::getShadowCasterReceiverBox() const {
        return shadowCasterReceiverBox;
    }

    const Matrix4<float>& LightSplitShadowMap::getLightProjectionMatrix() const {
        return lightProjectionMatrix;
    }

    /**
     * @return Models visible from light in frustum split
     */
    const std::vector<Model*>& LightSplitShadowMap::getModels() const {
        return models;
    }

    bool LightSplitShadowMap::needShadowMapUpdate() const {
        return shadowCasterReceiverBoxUpdated || modelsRequireUpdate;
    }

    /**
     * @return Box in light space containing shadow caster and receiver (scene independent)
     */
    AABBox<float> LightSplitShadowMap::createSceneIndependentBox(const Frustum<float>& splitFrustum, const Matrix4<float>& lightViewMatrix) const {
        ScopeProfiler sp(Profiler::graphic(), "sceneIndepBox");

        const Frustum<float>& frustumLightSpace = lightViewMatrix * splitFrustum;

        //determine point belonging to shadow caster/receiver box
        Point3<float> shadowReceiverAndCasterVertex[16];
        float nearCapZ = computeNearZForSceneIndependentBox(frustumLightSpace);
        for (std::size_t i = 0; i < 8; ++i) {
            const Point3<float>& frustumPoint = frustumLightSpace.getFrustumPoints()[i];

            //add shadow receiver points
            shadowReceiverAndCasterVertex[i * 2] = frustumPoint;

            //add shadow caster points
            shadowReceiverAndCasterVertex[i * 2 + 1] = Point3<float>(frustumPoint.X, frustumPoint.Y, nearCapZ);
        }

        //build shadow receiver/caster bounding box from points
        return AABBox<float>(shadowReceiverAndCasterVertex, 16);
    }

    float LightSplitShadowMap::computeNearZForSceneIndependentBox(const Frustum<float>& splitFrustumLightSpace) const {
        float nearestPointFromLight = splitFrustumLightSpace.getFrustumPoints()[0].Z;
        for (unsigned int i = 1; i < 8; ++i) {
            if (splitFrustumLightSpace.getFrustumPoints()[i].Z > nearestPointFromLight) {
                nearestPointFromLight = splitFrustumLightSpace.getFrustumPoints()[i].Z;
            }
        }
        return nearestPointFromLight + lightShadowMap->getViewingShadowDistance();
    }

    /**
     * Build box in light space containing shadow caster and receiver (scene dependent)
     */
    void LightSplitShadowMap::buildSceneDependentBox(const AABBox<float>& aabboxSceneIndependent, const OBBox<float>& obboxSceneIndependentViewSpace, bool forceUpdateAllShadowMap) {
        ScopeProfiler sp(Profiler::graphic(), "sceneDepBox");

        unsigned int modelsCount = 0;
        AABBox<float> modelsAabbox = AABBox<float>::initMergeableAABBox();

        for (const auto& model : models) {
            if (model->getSplitAABBoxes().size() == 1) {
                modelsAabbox = modelsAabbox.merge(lightShadowMap->getLightViewMatrix() * model->getSplitAABBoxes()[0]);
                modelsCount++;
            } else {
                for (const auto& splitAABBox : model->getSplitAABBoxes()) {
                    if (obboxSceneIndependentViewSpace.collideWithAABBox(splitAABBox)) {
                        modelsAabbox = modelsAabbox.merge(lightShadowMap->getLightViewMatrix() * splitAABBox);
                        modelsCount++;
                    }
                }
            }
        }

        AABBox<float> aabboxSceneDependent(Point3<float>(0.0f, 0.0f, 0.0f), Point3<float>(0.0f, 0.0f, 0.0f));
        if (modelsCount > 0) {
            Point3<float> cutMin(
                    std::min(std::max(modelsAabbox.getMin().X, aabboxSceneIndependent.getMin().X), aabboxSceneIndependent.getMax().X),
                    std::min(std::max(modelsAabbox.getMin().Y, aabboxSceneIndependent.getMin().Y), aabboxSceneIndependent.getMax().Y),
                    aabboxSceneIndependent.getMin().Z); //shadow can be projected outside the box: value cannot be capped

            Point3<float> cutMax(
                    std::max(std::min(modelsAabbox.getMax().X, aabboxSceneIndependent.getMax().X), aabboxSceneIndependent.getMin().X),
                    std::max(std::min(modelsAabbox.getMax().Y, aabboxSceneIndependent.getMax().Y), aabboxSceneIndependent.getMin().Y),
                    std::max(std::min(modelsAabbox.getMax().Z, aabboxSceneIndependent.getMax().Z), aabboxSceneIndependent.getMin().Z));

            aabboxSceneDependent = AABBox<float>(cutMin, cutMax);
        }

        //avoid aabbox of size zero when there is no model or when all models are outside of the aabboxSceneIndependent
        aabboxSceneDependent = AABBox<float>(aabboxSceneDependent.getMin() - LIGHT_BOX_MARGIN, aabboxSceneDependent.getMax() + LIGHT_BOX_MARGIN);

        updateShadowCasterReceiverBox(aabboxSceneDependent, forceUpdateAllShadowMap);
    }

    void LightSplitShadowMap::updateShadowCasterReceiverBox(const AABBox<float>& shadowCasterReceiverBox, bool forceUpdateAllShadowMap) {
        if (!forceUpdateAllShadowMap && areAlmostIdenticalAABBox(shadowCasterReceiverBox, this->shadowCasterReceiverBox)) {
            this->shadowCasterReceiverBoxUpdated = false;
        } else {
            this->shadowCasterReceiverBox = shadowCasterReceiverBox;
            this->lightProjectionMatrix = shadowCasterReceiverBox.toProjectionMatrix();

            this->shadowCasterReceiverBoxUpdated = true;
        }
    }

    bool LightSplitShadowMap::areAlmostIdenticalAABBox(const AABBox<float>& shadowCasterReceiverBox1, const AABBox<float>& shadowCasterReceiverBox2) const {
        float updateShadowMapSquareThreshold = updateShadowMapThreshold * updateShadowMapThreshold;

        return shadowCasterReceiverBox1.getMin().squareDistance(shadowCasterReceiverBox2.getMin()) < updateShadowMapSquareThreshold
               && shadowCasterReceiverBox1.getMax().squareDistance(shadowCasterReceiverBox2.getMax()) < updateShadowMapSquareThreshold;
    }

    /**
     * @models Models visible from light in frustum split
     */
    void LightSplitShadowMap::updateModels(const std::vector<Model*>& models) {
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
