#include <stdexcept>
#include <cmath>
#include <limits>

#include "OmnidirectionalLight.h"

namespace urchin {

    OmnidirectionalLight::OmnidirectionalLight(const Point3<float>& position) :
            Light(),
            position(position),
            attenuationNoEffect(ConfigService::instance()->getFloatValue("light.attenuationNoEffect")),
            exponentialAttenuation(0.01),
            sphereScope(nullptr),
            bboxScope(nullptr) {
        directions.emplace_back(Vector3<float>(1.0f, 0.0f, 0.0f)); //Left
        directions.emplace_back(Vector3<float>(-1.0f, 0.0f, 0.0f)); //Right
        directions.emplace_back(Vector3<float>(0.0f, 1.0f, 0.0f)); //Top
        directions.emplace_back(Vector3<float>(0.0f, -1.0f, 0.0f)); //Bottom
        directions.emplace_back(Vector3<float>(0.0f, 0.0f, 1.0f)); //Back
        directions.emplace_back(Vector3<float>(0.0f, 0.0f, -1.0f)); //Front

        computeScope();
    }

    OmnidirectionalLight::~OmnidirectionalLight() {
        delete sphereScope;
        delete bboxScope;
    }

    void OmnidirectionalLight::setPosition(const Point3<float>& position) {
        this->position = position;

        computeScope();

        notifyObservers(this, Light::LIGHT_MOVE);
    }

    const Point3<float>& OmnidirectionalLight::getPosition() const {
        return position;
    }

    const std::vector<Vector3<float>>& OmnidirectionalLight::getDirections() const {
        return directions;
    }

    Light::LightType OmnidirectionalLight::getLightType() const {
        return OMNIDIRECTIONAL;
    }

    bool OmnidirectionalLight::hasParallelBeams() const {
        return false;
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
    }

    float OmnidirectionalLight::getExponentialAttenuation() const {
        return exponentialAttenuation;
    }

    const AABBox<float>& OmnidirectionalLight::getAABBoxScope() const {
        return *bboxScope;
    }

    const Sphere<float>& OmnidirectionalLight::getSphereScope() const {
        return *sphereScope;
    }

    /**
     * Computes the sphere scope representing light affectation zone
     */
    void OmnidirectionalLight::computeScope() {
        delete sphereScope;
        delete bboxScope;

        float radius = -std::log(attenuationNoEffect) / getExponentialAttenuation();
        sphereScope = new Sphere<float>(radius, getPosition());
        bboxScope = new AABBox<float>(getPosition()-radius, getPosition()+radius);

        notifyOctreeableMove();
    }

}
