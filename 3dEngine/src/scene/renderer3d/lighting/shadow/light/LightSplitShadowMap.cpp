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

        //1. use prev center to recalibrate the matrix
        Point2<float> roundOffset = stabilizeShadows(lightProjectionMatrix * lightShadowMap->getLightViewMatrix(), previousCenter);
        lightProjectionMatrix.a14 += roundOffset.X;
        lightProjectionMatrix.a24 += roundOffset.Y;

        //2. compute previousCenter to match perfectly a pixel on shadow map
        previousCenter = Point4<float>(splitFrustum.computeCenterPosition(), 1.0f);
        previousCenter = stabilizeShadows2(lightProjectionMatrix * lightShadowMap->getLightViewMatrix(), previousCenter);




//TODO check stabilizeShadows2 is correct (adjusted and adjusted (2) must be equals)
//        previousCenter = Point4<float>(splitFrustum.computeCenterPosition(), 1.0f);
//        std::cout<<"Original: "<<previousCenter<<std::endl;
//        previousCenter = stabilizeShadows2(getLightProjectionMatrix() * lightShadowMap->getLightViewMatrix(), previousCenter);
//        std::cout<<"Ajusted: "<<previousCenter<<std::endl;
//        previousCenter = stabilizeShadows2(getLightProjectionMatrix() * lightShadowMap->getLightViewMatrix(), previousCenter);
//        std::cout<<"Ajusted (2): "<<previousCenter<<std::endl<<std::endl;

//TODO check stabilizeShadows is correct (offset (1) & (2) must be zero
//        previousCenter = Point4<float>(splitFrustum.computeCenterPosition(), 1.0f);
//        Point2<float> roundOffsetO = stabilizeShadows(getLightProjectionMatrix() * lightShadowMap->getLightViewMatrix(), previousCenter);
//        std::cout<<"Before offset: "<<roundOffsetO<<std::endl;
//        Matrix4<float> trans = Matrix4<float>::buildTranslation(roundOffsetO.X, roundOffsetO.Y, 0.0f);
//        Point2<float> roundOffset2 = stabilizeShadows(trans * getLightProjectionMatrix() * lightShadowMap->getLightViewMatrix(), previousCenter);
//        std::cout<<"After offset (1): "<<roundOffset2<<std::endl;
//        lightProjectionMatrix.a14 += roundOffsetO.X;
//        lightProjectionMatrix.a24 += roundOffsetO.Y;
//        Point2<float> roundOffset3 = stabilizeShadows(lightProjectionMatrix * lightShadowMap->getLightViewMatrix(), previousCenter);
//        std::cout<<"After offset (2): "<<roundOffset3<<std::endl<<std::endl;
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

    Point2<float> LightSplitShadowMap::stabilizeShadows(const Matrix4<float>& projViewMatrix, const Point4<float>& point) const {
        Point4<float> shadowTexCoord;
        Point3<float> roundedShadowTexCoord = computeRoundedShadowTexCoord(projViewMatrix, point, shadowTexCoord);

        Point2<float> roundOffset = Point2<float>(roundedShadowTexCoord.X, roundedShadowTexCoord.Y) - Point2<float>(shadowTexCoord.X, shadowTexCoord.Y);
        roundOffset = ((roundOffset / (float)lightShadowMap->getShadowMapSize()) * 2.0f);
        return roundOffset;
    }

    Point4<float> LightSplitShadowMap::stabilizeShadows2(const Matrix4<float>& projViewMatrix, const Point4<float>& point) const {
        Point4<float> shadowTexCoord;
        Point3<float> roundedShadowTexCoord = computeRoundedShadowTexCoord(projViewMatrix, point, shadowTexCoord);

        roundedShadowTexCoord = (roundedShadowTexCoord / (float)lightShadowMap->getShadowMapSize() * 2.0f) - 1.0f;
        return projViewMatrix.inverse() * Point4<float>(roundedShadowTexCoord.X, roundedShadowTexCoord.Y, roundedShadowTexCoord.Z, 1.0f);
    }

    /**
     * @param shadowTexCoord [out] Shadow texture coordinate (not rounded)
     */
    Point3<float> LightSplitShadowMap::computeRoundedShadowTexCoord(const Matrix4<float>& projViewMatrix, const Point4<float>& point, Point4<float>& shadowTexCoord) const {
        float halfPixelSize = (0.5f / (float)lightShadowMap->getShadowMapSize()); //TODO or 0.0f ?

        shadowTexCoord = projViewMatrix * point; //coordinates from -1.0 to 1.0
        shadowTexCoord = (shadowTexCoord + 1.0f) * 0.5f * (float)lightShadowMap->getShadowMapSize(); //coordinates from 0 to shadowMapSize

        return Point3<float>(std::round(shadowTexCoord.X + halfPixelSize) - halfPixelSize, std::round(shadowTexCoord.Y + halfPixelSize) - halfPixelSize, shadowTexCoord.Z);
    }

}
