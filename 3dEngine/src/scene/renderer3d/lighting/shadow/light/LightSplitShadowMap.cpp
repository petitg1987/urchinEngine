#include <scene/renderer3d/lighting/shadow/light/LightSplitShadowMap.h>
#include <scene/renderer3d/lighting/shadow/light/LightShadowMap.h>
#include <scene/renderer3d/lighting/light/omnidirectional/OmnidirectionalLight.h>
#include <scene/renderer3d/lighting/light/spot/SpotLight.h>

namespace urchin {

    LightSplitShadowMap::LightSplitShadowMap(unsigned int splitIndex, const LightShadowMap* lightShadowMap) :
            splitIndex(splitIndex),
            lightShadowMap(lightShadowMap),
            previousCenter(Point4(0.0f, 0.0f, 0.0f, 1.0f)),
            nearPlane(std::numeric_limits<float>::max()),
            farPlane(std::numeric_limits<float>::max()) {
        Light::LightType lightType = lightShadowMap->getLight().getLightType();
        if (lightType == Light::LightType::SUN) {
            lightScopeConvexObject = std::make_unique<OBBox<float>>();
        } else if (lightType == Light::LightType::OMNIDIRECTIONAL) {
            lightScopeConvexObject = std::make_unique<Sphere<float>>();
        } else if (lightType == Light::LightType::SPOT) {
            lightScopeConvexObject = std::make_unique<Frustum<float>>();
        } else {
            throw std::invalid_argument("Unknown light type to initialize light scope convex object: " + std::to_string((int)lightType));
        }

        onLightAffectedZoneUpdated();
    }

    void LightSplitShadowMap::onLightAffectedZoneUpdated() {
        updateLightViewMatrix();

        Light::LightType lightType = lightShadowMap->getLight().getLightType();
        if (lightType == Light::LightType::OMNIDIRECTIONAL) {
            updateOmnidirectionalLightScopeData();
        } else if (lightType == Light::LightType::SPOT) {
            updateSpotLightScopeData();
        }
    }

    void LightSplitShadowMap::onSplitFrustumUpdated(const SplitFrustum& splitFrustum) {
        Light::LightType lightType = lightShadowMap->getLight().getLightType();
        if (lightType == Light::LightType::SUN) {
            updateSunLightScopeData(splitFrustum);
        }
    }

    void LightSplitShadowMap::updateLightViewMatrix() {
        Light::LightType lightType = lightShadowMap->getLight().getLightType();

        Vector3<float> lightDirection = lightShadowMap->getLight().getDirections()[0];
        if (lightType == Light::LightType::OMNIDIRECTIONAL) {
            lightDirection = lightShadowMap->getLight().getDirections()[splitIndex];
        }

        Vector3<float> forward = lightDirection.normalize();
        Vector3 worldUp(0.0f, 1.0f, 0.0f);
        if (std::abs(forward.dotProduct(worldUp)) > 0.999f) {
            worldUp = Vector3(1.0f, 0.0f, 0.0f);
        }
        Vector3<float> side = forward.crossProduct(worldUp).normalize();
        Vector3<float> up = side.crossProduct(forward).normalize();

        float translationOnSide = 0.0f;
        float translationOnUp = 0.0f;
        float translationOnForward = 0.0f;
        if (lightType == Light::LightType::SUN) {
            translationOnSide = lightDirection.X;
            translationOnUp = lightDirection.Y;
            translationOnForward = lightDirection.Z;
        } else {
            assert(lightType == Light::LightType::SPOT || lightType == Light::LightType::OMNIDIRECTIONAL);
            Vector3<float> pos = lightShadowMap->getLight().getPosition().toVector();
            translationOnSide = -side.dotProduct(pos);
            translationOnUp = -up.dotProduct(pos);
            translationOnForward = forward.dotProduct(pos);
        }

        this->lightViewMatrix.setValues(
                side[0],        side[1],        side[2],        translationOnSide,
                up[0],          up[1],          up[2],          translationOnUp,
                -forward[0],    -forward[1],    -forward[2],    translationOnForward,
                0.0f,           0.0f,           0.0f,           1.0f);
    }

    void LightSplitShadowMap::updateSunLightScopeData(const SplitFrustum& splitFrustum) {
        const Frustum<float>& frustumLightSpace = lightViewMatrix * splitFrustum.getFrustum();
        Point3<float> splitFrustumCenter = (lightViewMatrix * Point4(splitFrustum.getBoundingSphere().getCenterOfMass(), 1.0f)).toPoint3();
        float splitFrustumRadius = splitFrustum.getBoundingSphere().getRadius();
        float nearCapZ = computeNearZForSceneIndependentBox(frustumLightSpace);

        std::array<Point3<float>, 4> lightProjectionVertex;
        lightProjectionVertex[0] = splitFrustumCenter + Point3(splitFrustumRadius, splitFrustumRadius, splitFrustumRadius);
        lightProjectionVertex[1] = Point3(lightProjectionVertex[0].X, lightProjectionVertex[0].Y, nearCapZ);
        lightProjectionVertex[2] = splitFrustumCenter - Point3(splitFrustumRadius, splitFrustumRadius, splitFrustumRadius);
        lightProjectionVertex[3] = Point3(lightProjectionVertex[2].X, lightProjectionVertex[2].Y, nearCapZ);
        lightProjectionMatrix = AABBox<float>(lightProjectionVertex).toProjectionMatrix();
        stabilizeShadow(splitFrustum.getFrustum().computeCenterPosition());

        //determine point belonging to shadow caster/receiver box
        std::array<Point3<float>, 16> shadowReceiverAndCasterVertex;
        for (std::size_t i = 0; i < 8; ++i) {
            const Point3<float>& frustumPoint = frustumLightSpace.getFrustumPoints()[i];

            shadowReceiverAndCasterVertex[i * 2] = frustumPoint; //shadow receiver point
            shadowReceiverAndCasterVertex[i * 2 + 1] = Point3(frustumPoint.X, frustumPoint.Y, nearCapZ); //shadow caster point
        }
        AABBox<float> shadowCasterReceiverShape(shadowReceiverAndCasterVertex);
        OBBox<float> obboxSceneIndependentViewSpace = lightViewMatrix.inverse() * OBBox(shadowCasterReceiverShape);

        auto oldObbox = dynamic_cast<OBBox<float>*>(lightScopeConvexObject.get());
        *oldObbox = obboxSceneIndependentViewSpace;
    }

    void LightSplitShadowMap::updateOmnidirectionalLightScopeData() {
        //const auto& omnidirectionalLight = static_cast<OmnidirectionalLight&>(lightShadowMap->getLight());

        //TODO impl
    }

    void LightSplitShadowMap::updateSpotLightScopeData() {
        const auto& spotLight = static_cast<SpotLight&>(lightShadowMap->getLight());

        float ratio = 1.0f;
        float tanFov = std::tan(AngleConverter<float>::toRadian(spotLight.getOuterAngle()));
        nearPlane = SpotLight::FRUSTUM_NEAR_PLANE;
        farPlane = spotLight.getFrustumScope().computeFarDistance();

        lightProjectionMatrix.setValues(
                1.0f / (tanFov * ratio), 0.0f, 0.0f, 0.0f,
                0.0f, -1.0f / tanFov, 0.0f, 0.0f,
                0.0f, 0.0f, farPlane / (nearPlane - farPlane), (farPlane * nearPlane) / (nearPlane - farPlane),
                0.0f, 0.0f, -1.0f, 0.0f);

        auto oldFrustum = dynamic_cast<Frustum<float>*>(lightScopeConvexObject.get());
        *oldFrustum = spotLight.getFrustumScope();
    }

    void LightSplitShadowMap::updateVisibleModels() {
        models.clear();
        lightShadowMap->getModelOcclusionCuller().getModelsInConvexObject(*lightScopeConvexObject, models, true, [](const Model *const model) {
            return model->getShadowBehavior() == Model::ShadowBehavior::RECEIVER_AND_CASTER;
        });
    }

    const Matrix4<float>& LightSplitShadowMap::getLightProjectionMatrix() const {
        return lightProjectionMatrix;
    }

    const Matrix4<float>& LightSplitShadowMap::getLightViewMatrix() const {
        return lightViewMatrix;
    }

    float LightSplitShadowMap::getNearPlane() const {
        #ifdef URCHIN_DEBUG
            assert(lightShadowMap->getLight().getLightType() == Light::LightType::SPOT || lightShadowMap->getLight().getLightType() == Light::LightType::OMNIDIRECTIONAL);
        #endif
        return nearPlane;
    }

    float LightSplitShadowMap::getFarPlane() const {
        #ifdef URCHIN_DEBUG
            assert(lightShadowMap->getLight().getLightType() == Light::LightType::SPOT || lightShadowMap->getLight().getLightType() == Light::LightType::OMNIDIRECTIONAL);
        #endif
        return farPlane;
    }

    /**
     * @return Models visible from light in frustum split
     */
    std::span<Model* const> LightSplitShadowMap::getModels() const {
        return models;
    }

    float LightSplitShadowMap::computeNearZForSceneIndependentBox(const Frustum<float>& splitFrustumLightSpace) const {
        float nearestPointFromLight = splitFrustumLightSpace.getFrustumPoints()[0].Z;
        for (unsigned int i = 1; i < 8; ++i) {
            if (splitFrustumLightSpace.getFrustumPoints()[i].Z > nearestPointFromLight) {
                nearestPointFromLight = splitFrustumLightSpace.getFrustumPoints()[i].Z;
            }
        }
        return nearestPointFromLight + lightShadowMap->getShadowViewDistance();
    }

    void LightSplitShadowMap::stabilizeShadow(const Point3<float>& splitFrustumCenter) {
        //1. use previousCenter to recalibrate the matrix
        Point2<float> centerWorldPointDelta = computePixelCenteringDelta(lightProjectionMatrix * lightViewMatrix, previousCenter);
        lightProjectionMatrix.a14 += centerWorldPointDelta.X;
        lightProjectionMatrix.a24 += centerWorldPointDelta.Y;

        //2. compute previousCenter to match perfectly a pixel on the shadow map
        previousCenter = Point4(splitFrustumCenter, 1.0f);
        previousCenter = adjustPointOnShadowMapPixel(lightProjectionMatrix * lightViewMatrix, previousCenter);
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
