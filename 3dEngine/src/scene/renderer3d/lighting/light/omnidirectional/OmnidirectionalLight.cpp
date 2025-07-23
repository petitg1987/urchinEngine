#include <stdexcept>
#include <cmath>
#include <limits>

#include "scene/renderer3d/lighting/light/omnidirectional/OmnidirectionalLight.h"

namespace urchin {

    OmnidirectionalLight::OmnidirectionalLight(const Point3<float>& position) :
            position(position),
            exponentialAttenuation(0.1f),
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

    void OmnidirectionalLight::setAttenuation(float exponentialAttenuation) {
        if (exponentialAttenuation < std::numeric_limits<float>::epsilon()) {
            throw std::domain_error("Exponential attenuation must be greater than zero.");
        }
        this->exponentialAttenuation = exponentialAttenuation;

        computeScope();

        notifyObservers(this, AFFECTED_ZONE_UPDATED);
        notifyObservers(this, ILLUMINATED_AREA_SIZE_UPDATED);
    }

    float OmnidirectionalLight::getExponentialAttenuation() const {
        return exponentialAttenuation;
    }

    float OmnidirectionalLight::computeRadius() const {
        return -std::log(ATTENUATION_NO_EFFECT) / getExponentialAttenuation();
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
        float radius = computeRadius();
        sphereScope = std::make_optional<Sphere<float>>(radius, getPosition());
        bboxScope = std::make_optional<AABBox<float>>(getPosition() - radius, getPosition() + radius);

        frustumScopes = std::make_optional<std::array<Frustum<float>, 6>>();
        for (unsigned int i = 0; i < directions.size(); ++i) {
            float nearPlane = FRUSTUM_NEAR_PLANE;
            float farPlane = computeRadius() + nearPlane;
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
