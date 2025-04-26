#include <stdexcept>
#include <cmath>
#include <limits>

#include <scene/renderer3d/lighting/light/omnidirectional/OmnidirectionalLight.h>

namespace urchin {

    OmnidirectionalLight::OmnidirectionalLight(const Point3<float>& position) :
            position(position),
            exponentialAttenuation(0.1f),
            sphereScope(nullptr),
            bboxScope(nullptr) {
        directions.emplace_back(1.0f, 0.0f, 0.0f); //Left
        directions.emplace_back(-1.0f, 0.0f, 0.0f); //Right
        directions.emplace_back(0.0f, 1.0f, 0.0f); //Top
        directions.emplace_back(0.0f, -1.0f, 0.0f); //Bottom
        directions.emplace_back(0.0f, 0.0f, 1.0f); //Back
        directions.emplace_back(0.0f, 0.0f, -1.0f); //Front

        computeScope();
    }

    void OmnidirectionalLight::setPosition(const Point3<float>& position) {
        this->position = position;

        computeScope();

        notifyObservers(this, LIGHT_MOVE);
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
    }

    float OmnidirectionalLight::getExponentialAttenuation() const {
        return exponentialAttenuation;
    }

    const AABBox<float>& OmnidirectionalLight::getAABBoxScope() const {
        assert(bboxScope);
        return *bboxScope;
    }

    const Sphere<float>& OmnidirectionalLight::getSphereScope() const {
        assert(sphereScope);
        return *sphereScope;
    }

    /**
     * Computes the sphere scope representing light affectation area
     */
    void OmnidirectionalLight::computeScope() {
        float radius = -std::log(ATTENUATION_NO_EFFECT) / getExponentialAttenuation();
        sphereScope = std::make_unique<Sphere<float>>(radius, getPosition());
        bboxScope = std::make_unique<AABBox<float>>(getPosition() - radius, getPosition() + radius);

        notifyOctreeableMove();
    }

    std::unique_ptr<Light> OmnidirectionalLight::clone() const {
        auto cloned = std::make_unique<OmnidirectionalLight>(position);
        cloned->setAttenuation(exponentialAttenuation);
        return cloned;
    }

}
