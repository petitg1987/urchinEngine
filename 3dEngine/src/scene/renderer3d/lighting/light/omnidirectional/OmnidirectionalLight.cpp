#include "scene/renderer3d/lighting/light/omnidirectional/OmnidirectionalLight.h"

namespace urchin {

    OmnidirectionalLight::OmnidirectionalLight(const Point3<float>& position) :
            position(position),
            scopeRadius(5.0f),
            falloffExponent(3.0f),
            sphereScope(std::nullopt),
            bboxScope(std::nullopt),
            frustumScopes(std::nullopt) {
        directions.emplace_back(1.0f, 0.0f, 0.0f); //Right (X+)
        directions.emplace_back(-1.0f, 0.0f, 0.0f); //Left (X-)
        directions.emplace_back(0.0f, 1.0f, 0.0f); //Top (Y+)
        directions.emplace_back(0.0f, -1.0f, 0.0f); //Bottom (Y-)
        directions.emplace_back(0.0f, 0.0f, 1.0f); //Front (Z+)
        directions.emplace_back(0.0f, 0.0f, -1.0f); //Back (Z-)

        computeScope();
    }

    void OmnidirectionalLight::setPosition(const Point3<float>& position) {
        this->position = position;

        computeScope();

        notifyObservers(this, AFFECTED_ZONE_UPDATED);
    }

    const Point3<float>& OmnidirectionalLight::getPosition() const {
        return position;
    }

    const std::vector<Vector3<float>>& OmnidirectionalLight::getDirections() const {
        return directions;
    }

    Light::LightType OmnidirectionalLight::getLightType() const {
        return LightType::OMNIDIRECTIONAL;
    }

    std::string OmnidirectionalLight::getLightTypeName() const {
        return "omnidirectional";
    }

    const AABBox<float>& OmnidirectionalLight::getAABBox() const {
        return *bboxScope;
    }

    void OmnidirectionalLight::setScopeRadius(float scopeRadius) {
        this->scopeRadius = scopeRadius;

        computeScope();
        notifyObservers(this, AFFECTED_ZONE_UPDATED);
    }

    float OmnidirectionalLight::getScopeRadius() const {
        return scopeRadius;
    }

    void OmnidirectionalLight::setFalloffExponent(float falloffExponent) {
        this->falloffExponent = falloffExponent;
    }

    float OmnidirectionalLight::getFalloffExponent() const {
        return falloffExponent;
    }

    const AABBox<float>& OmnidirectionalLight::getAABBoxScope() const {
        assert(bboxScope);
        return *bboxScope;
    }

    const Sphere<float>& OmnidirectionalLight::getSphereScope() const {
        assert(sphereScope);
        return *sphereScope;
    }

    const Frustum<float>& OmnidirectionalLight::getFrustumScope(unsigned int index) const {
        assert(frustumScopes);
        return frustumScopes.value()[index];
    }

    void OmnidirectionalLight::computeScope() {
        sphereScope = std::make_optional<Sphere<float>>(scopeRadius, getPosition());
        bboxScope = std::make_optional<AABBox<float>>(getPosition() - scopeRadius, getPosition() + scopeRadius);

        frustumScopes = std::make_optional<std::array<Frustum<float>, 6>>();
        for (unsigned int i = 0; i < directions.size(); ++i) {
            float nearPlane = FRUSTUM_NEAR_PLANE;
            float farPlane = scopeRadius + nearPlane;
            Quaternion<float> orientation = Quaternion<float>::rotationFromTo(Vector3(0.0f, 0.0f, -1.0f), directions[i]).normalize();
            Matrix4<float> transformMatrix = Matrix4<float>::buildTranslation(getPosition().X, getPosition().Y, getPosition().Z) * orientation.toMatrix4();
            Frustum frustum(90.0f, 1.0f, nearPlane, farPlane);
            frustumScopes.value()[i] = transformMatrix * frustum;
        }

        notifyOctreeableMove();
    }

    std::unique_ptr<Light> OmnidirectionalLight::clone() const {
        return std::make_unique<OmnidirectionalLight>(*this);
    }

}
