#include <scene/renderer3d/lighting/shadow/light/LightSplitShadowMap.h>
#include <scene/renderer3d/lighting/shadow/light/LightShadowMap.h>

namespace urchin {

    LightSplitShadowMap::LightSplitShadowMap(const LightShadowMap* lightShadowMap) :
            lightShadowMap(lightShadowMap),
            useSceneDependentProjection(ConfigService::instance().getBoolValue("shadow.useSceneDependentProjection")),
            previousCenter(Point4<float>(0.0f, 0.0f, 0.0f, 1.0f)) {

    }

    void LightSplitShadowMap::update(const Frustum<float>& splitFrustum) {
        AABBox<float> aabboxSceneIndependent = createSceneIndependentBox(splitFrustum, lightShadowMap->getLightViewMatrix());
        OBBox<float> obboxSceneIndependentViewSpace = lightShadowMap->getLightViewMatrix().inverse() * OBBox<float>(aabboxSceneIndependent);

        models.clear();
        lightShadowMap->getModelOcclusionCuller().getModelsInOBBox(obboxSceneIndependentViewSpace, models, [](const Model *const model) {
            //receiver only are required for variance shadow map to work correctly (see 8.4.5: https://developer.nvidia.com/gpugems/gpugems3/part-ii-light-and-shadows/chapter-8-summed-area-variance-shadow-maps)
            return model->getShadowBehavior() == Model::ShadowBehavior::RECEIVER_AND_CASTER || model->getShadowBehavior() == Model::ShadowBehavior::RECEIVER_ONLY;
        });

        if (useSceneDependentProjection) {
            AABBox<float> aabboxSceneDependent = buildSceneDependentBox(aabboxSceneIndependent, obboxSceneIndependentViewSpace);
            updateShadowCasterReceiverBox(aabboxSceneDependent);
        } else {
            updateShadowCasterReceiverBox(aabboxSceneIndependent);
        }

        stabilizeShadow(splitFrustum.computeCenterPosition());
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
    std::span<Model* const> LightSplitShadowMap::getModels() const {
        return models;
    }

    /**
     * @return Box in light space containing shadow caster and receiver (scene independent)
     */
    AABBox<float> LightSplitShadowMap::createSceneIndependentBox(const Frustum<float>& splitFrustum, const Matrix4<float>& lightViewMatrix) const {
        ScopeProfiler sp(Profiler::graphic(), "sceneIndepBox");

        const Frustum<float>& frustumLightSpace = lightViewMatrix * splitFrustum;

        //determine point belonging to shadow caster/receiver box
        std::array<Point3<float>, 16> shadowReceiverAndCasterVertex;
        float nearCapZ = computeNearZForSceneIndependentBox(frustumLightSpace);
        for (std::size_t i = 0; i < 8; ++i) {
            const Point3<float>& frustumPoint = frustumLightSpace.getFrustumPoints()[i];

            //add shadow receiver points
            shadowReceiverAndCasterVertex[i * 2] = frustumPoint;

            //add shadow caster points
            shadowReceiverAndCasterVertex[i * 2 + 1] = Point3<float>(frustumPoint.X, frustumPoint.Y, nearCapZ);
        }

        //build shadow receiver/caster bounding box from points
        return AABBox<float>(shadowReceiverAndCasterVertex);
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
    AABBox<float> LightSplitShadowMap::buildSceneDependentBox(const AABBox<float>& aabboxSceneIndependent, const OBBox<float>& obboxSceneIndependentViewSpace) const {
        ScopeProfiler sp(Profiler::graphic(), "sceneDepBox");

        unsigned int modelsCount = 0;
        AABBox<float> modelsAabbox = AABBox<float>::initMergeableAABBox();

        for (const auto& model : models) {
            if (model->getShadowBehavior() == Model::ShadowBehavior::RECEIVER_ONLY) [[unlikely]] {
                continue;
            }

            if (model->getSplitAABBoxes().size() == 1) [[likely]] {
                modelsAabbox = modelsAabbox.merge(lightShadowMap->getLightViewMatrix() * model->getSplitAABBoxes()[0]);
                modelsCount++;
            } else {
                for (const auto& splitAABBox : model->getSplitAABBoxes()) {
                    if (obboxSceneIndependentViewSpace.toAABBox().collideWithAABBox(splitAABBox)) {
                        modelsAabbox = modelsAabbox.merge(lightShadowMap->getLightViewMatrix() * splitAABBox);
                        modelsCount++;
                    }
                }
            }
        }

        AABBox aabboxSceneDependent(Point3<float>(0.0f, 0.0f, 0.0f), Point3<float>(0.0f, 0.0f, 0.0f));
        if (modelsCount > 0) {
            Point3 cutMin(
                    std::min(std::max(modelsAabbox.getMin().X, aabboxSceneIndependent.getMin().X), aabboxSceneIndependent.getMax().X),
                    std::min(std::max(modelsAabbox.getMin().Y, aabboxSceneIndependent.getMin().Y), aabboxSceneIndependent.getMax().Y),
                    aabboxSceneIndependent.getMin().Z); //shadow can be projected outside the box: value cannot be capped

            Point3 cutMax(
                    std::max(std::min(modelsAabbox.getMax().X, aabboxSceneIndependent.getMax().X), aabboxSceneIndependent.getMin().X),
                    std::max(std::min(modelsAabbox.getMax().Y, aabboxSceneIndependent.getMax().Y), aabboxSceneIndependent.getMin().Y),
                    std::max(std::min(modelsAabbox.getMax().Z, aabboxSceneIndependent.getMax().Z), aabboxSceneIndependent.getMin().Z));

            aabboxSceneDependent = AABBox<float>(cutMin, cutMax);
        }

        //avoid aabbox of size zero when there is no model or when all models are outside the aabboxSceneIndependent
        return AABBox<float>(aabboxSceneDependent.getMin() - LIGHT_BOX_MARGIN, aabboxSceneDependent.getMax() + LIGHT_BOX_MARGIN);
    }

    void LightSplitShadowMap::updateShadowCasterReceiverBox(const AABBox<float>& shadowCasterReceiverBox) {
        this->shadowCasterReceiverBox = shadowCasterReceiverBox;
        this->lightProjectionMatrix = shadowCasterReceiverBox.toProjectionMatrix();
    }

    void LightSplitShadowMap::stabilizeShadow(const Point3<float>& splitFrustumCenter) {
        //1. use previousCenter to recalibrate the matrix
        Point2<float> centerWorldPointDelta = computePixelCenteringDelta(lightProjectionMatrix * lightShadowMap->getLightViewMatrix(), previousCenter);
        lightProjectionMatrix.a14 += centerWorldPointDelta.X;
        lightProjectionMatrix.a24 += centerWorldPointDelta.Y;

        //2. compute previousCenter to match perfectly a pixel on shadow map
        previousCenter = Point4<float>(splitFrustumCenter, 1.0f);
        previousCenter = adjustPointOnShadowMapPixel(lightProjectionMatrix * lightShadowMap->getLightViewMatrix(), previousCenter);
    }

    Point2<float> LightSplitShadowMap::computePixelCenteringDelta(const Matrix4<float>& lightProjectionViewMatrix, const Point4<float>& worldPoint) const {
        const auto[shadowTexCoord, shadowTexCoordOnPixelCenter] = computeShadowTexCoords(lightProjectionViewMatrix, worldPoint);
        Point2<float> centerWorldPointDelta = Point2<float>(shadowTexCoordOnPixelCenter.X, shadowTexCoordOnPixelCenter.Y) - Point2<float>(shadowTexCoord.X, shadowTexCoord.Y);
        centerWorldPointDelta = ((centerWorldPointDelta / (float)lightShadowMap->getShadowMapSize()) * 2.0f);
        return centerWorldPointDelta;
    }

    Point4<float> LightSplitShadowMap::adjustPointOnShadowMapPixel(const Matrix4<float>& lightProjectionViewMatrix, const Point4<float>& worldPoint) const {
        Point3<float> shadowTexCoordOnPixelCenter = computeShadowTexCoords(lightProjectionViewMatrix, worldPoint).second;
        shadowTexCoordOnPixelCenter = (shadowTexCoordOnPixelCenter / (float)lightShadowMap->getShadowMapSize() * 2.0f) - 1.0f;
        return lightProjectionViewMatrix.inverse() * Point4<float>(shadowTexCoordOnPixelCenter.X, shadowTexCoordOnPixelCenter.Y, shadowTexCoordOnPixelCenter.Z, 1.0f);
    }

    std::pair<Point4<float>, Point3<float>> LightSplitShadowMap::computeShadowTexCoords(const Matrix4<float>& lightProjectionViewMatrix, const Point4<float>& worldPoint) const {
        Point4<float> shadowTexCoord = lightProjectionViewMatrix * worldPoint; //coordinates from -1.0 to 1.0
        shadowTexCoord = (shadowTexCoord + 1.0f) * 0.5f * (float)lightShadowMap->getShadowMapSize(); //coordinates from 0 to shadowMapSize

        float halfPixelSize = 0.5f;
        Point3<float> shadowTexCoordOnPixelCenter(std::round(shadowTexCoord.X + halfPixelSize) - halfPixelSize, std::round(shadowTexCoord.Y + halfPixelSize) - halfPixelSize, shadowTexCoord.Z);

        return std::make_pair(shadowTexCoord, shadowTexCoordOnPixelCenter);
    }

}
