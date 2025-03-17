#include <scene/renderer3d/lighting/shadow/light/LightSplitShadowMap.h>
#include <scene/renderer3d/lighting/shadow/light/LightShadowMap.h>

#include "scene/renderer3d/lighting/light/spot/SpotLight.h"

namespace urchin {

    LightSplitShadowMap::LightSplitShadowMap(const LightShadowMap* lightShadowMap) :
            lightShadowMap(lightShadowMap),
            previousCenter(Point4(0.0f, 0.0f, 0.0f, 1.0f)) {

    }

    void LightSplitShadowMap::update(const SplitFrustum& splitFrustum) {
        computeLightScope(splitFrustum, lightShadowMap->getLightViewMatrix());
        stabilizeShadow(splitFrustum.getFrustum().computeCenterPosition());

        models.clear();
        OBBox<float> obboxSceneIndependentViewSpace = lightShadowMap->getLightViewMatrix().inverse() * OBBox(shadowCasterReceiverBox);
        lightShadowMap->getModelOcclusionCuller().getModelsInOBBox(obboxSceneIndependentViewSpace, models, [](const Model *const model) {
            return model->getShadowBehavior() == Model::ShadowBehavior::RECEIVER_AND_CASTER;
        });
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

    void LightSplitShadowMap::computeLightScope(const SplitFrustum& splitFrustum, const Matrix4<float>& lightViewMatrix) {
        ScopeProfiler sp(Profiler::graphic(), "compLightScope");

        if (lightShadowMap->getLight().getLightType() == Light::LightType::SUN) {
            const Frustum<float>& frustumLightSpace = lightViewMatrix * splitFrustum.getFrustum();
            Point3<float> frustumCenter = (lightViewMatrix * Point4(splitFrustum.getBoundingSphere().getCenterOfMass(), 1.0f)).toPoint3();
            float frustumRadius = splitFrustum.getBoundingSphere().getRadius();
            float nearCapZ = computeNearZForSceneIndependentBox(frustumLightSpace);

            std::array<Point3<float>, 4> lightProjectionVertex;
            lightProjectionVertex[0] = frustumCenter + Point3(frustumRadius, frustumRadius, frustumRadius);
            lightProjectionVertex[1] = Point3(lightProjectionVertex[0].X, lightProjectionVertex[0].Y, nearCapZ);
            lightProjectionVertex[2] = frustumCenter - Point3(frustumRadius, frustumRadius, frustumRadius);
            lightProjectionVertex[3] = Point3(lightProjectionVertex[2].X, lightProjectionVertex[2].Y, nearCapZ);
            this->lightProjectionMatrix = AABBox<float>(lightProjectionVertex).toProjectionMatrix();

            //determine point belonging to shadow caster/receiver box
            std::array<Point3<float>, 16> shadowReceiverAndCasterVertex;
            for (std::size_t i = 0; i < 8; ++i) {
                const Point3<float>& frustumPoint = frustumLightSpace.getFrustumPoints()[i];

                shadowReceiverAndCasterVertex[i * 2] = frustumPoint; //shadow receiver point
                shadowReceiverAndCasterVertex[i * 2 + 1] = Point3(frustumPoint.X, frustumPoint.Y, nearCapZ); //shadow caster point
            }
            this->shadowCasterReceiverBox = AABBox<float>(shadowReceiverAndCasterVertex);
        } else if (lightShadowMap->getLight().getLightType() == Light::LightType::SPOT) {
            const auto& spotLight = static_cast<SpotLight&>(lightShadowMap->getLight());

            // float verticalFovAngle = computeVerticalFovAngle();
            // float fov = computeFov(verticalFovAngle);
            // float ratio = (float)sceneWidth / (float)sceneHeight;
            // mProjection.setValues(
            //         fov / ratio, 0.0f, 0.0f, 0.0f,
            //         0.0f, -fov, 0.0f, 0.0f,
            //         0.0f, 0.0f, 0.5f * ((farPlane + nearPlane) / (nearPlane - farPlane)) - 0.5f, (farPlane * nearPlane) / (nearPlane - farPlane),
            //         0.0f, 0.0f, -1.0f, 0.0f);
            // --------- OR ----------
            // - Compute: projection matrix: glm::perspective(glm::radians(lightFOV), 1.0f, zNear, zFar)

            float nearPlane = 0.1f;
            float farPlane = 50.0f;
            //float verticalFovAngle = 0.0f; //TODO computeVerticalFovAngle();
            float fov = AngleConverter<float>::toRadian(110.0f); //TODO computeFov(verticalFovAngle);
            float ratio = 1.0f;
            this->lightProjectionMatrix.setValues(
                    fov / ratio, 0.0f, 0.0f, 0.0f,
                    0.0f, -fov, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.5f * ((farPlane + nearPlane) / (nearPlane - farPlane)) - 0.5f, (farPlane * nearPlane) / (nearPlane - farPlane),
                    0.0f, 0.0f, -1.0f, 0.0f);

            this->shadowCasterReceiverBox = spotLight.getAABBoxScope();
        } else {
            throw std::runtime_error("Shadow currently not supported for light of type: " + std::to_string((int)lightShadowMap->getLight().getLightType()));
        }
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

    void LightSplitShadowMap::stabilizeShadow(const Point3<float>& splitFrustumCenter) {
        //1. use previousCenter to recalibrate the matrix
        Point2<float> centerWorldPointDelta = computePixelCenteringDelta(lightProjectionMatrix * lightShadowMap->getLightViewMatrix(), previousCenter);
        lightProjectionMatrix.a14 += centerWorldPointDelta.X;
        lightProjectionMatrix.a24 += centerWorldPointDelta.Y;

        //2. compute previousCenter to match perfectly a pixel on shadow map
        previousCenter = Point4(splitFrustumCenter, 1.0f);
        previousCenter = adjustPointOnShadowMapPixel(lightProjectionMatrix * lightShadowMap->getLightViewMatrix(), previousCenter);
    }

    Point2<float> LightSplitShadowMap::computePixelCenteringDelta(const Matrix4<float>& lightProjectionViewMatrix, const Point4<float>& worldPoint) const {
        const auto[shadowTexCoord, shadowTexCoordOnPixelCenter] = computeShadowTexCoords(lightProjectionViewMatrix, worldPoint);
        Point2<float> centerWorldPointDelta = Point2(shadowTexCoordOnPixelCenter.X, shadowTexCoordOnPixelCenter.Y) - Point2(shadowTexCoord.X, shadowTexCoord.Y);
        centerWorldPointDelta = ((centerWorldPointDelta / (float)lightShadowMap->getShadowMapSize()) * 2.0f);
        return centerWorldPointDelta;
    }

    Point4<float> LightSplitShadowMap::adjustPointOnShadowMapPixel(const Matrix4<float>& lightProjectionViewMatrix, const Point4<float>& worldPoint) const {
        Point3<float> shadowTexCoordOnPixelCenter = computeShadowTexCoords(lightProjectionViewMatrix, worldPoint).second;
        shadowTexCoordOnPixelCenter = (shadowTexCoordOnPixelCenter / (float)lightShadowMap->getShadowMapSize() * 2.0f) - 1.0f;
        return lightProjectionViewMatrix.inverse() * Point4(shadowTexCoordOnPixelCenter.X, shadowTexCoordOnPixelCenter.Y, shadowTexCoordOnPixelCenter.Z, 1.0f);
    }

    std::pair<Point4<float>, Point3<float>> LightSplitShadowMap::computeShadowTexCoords(const Matrix4<float>& lightProjectionViewMatrix, const Point4<float>& worldPoint) const {
        Point4<float> shadowTexCoord = lightProjectionViewMatrix * worldPoint; //coordinates from -1.0 to 1.0
        shadowTexCoord = (shadowTexCoord + 1.0f) * 0.5f * (float)lightShadowMap->getShadowMapSize(); //coordinates from 0 to shadowMapSize

        float halfPixelSize = 0.5f;
        Point3 shadowTexCoordOnPixelCenter(std::round(shadowTexCoord.X + halfPixelSize) - halfPixelSize, std::round(shadowTexCoord.Y + halfPixelSize) - halfPixelSize, shadowTexCoord.Z);

        return std::make_pair(shadowTexCoord, shadowTexCoordOnPixelCenter);
    }

}
