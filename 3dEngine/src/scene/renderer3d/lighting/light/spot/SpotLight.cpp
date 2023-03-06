#include <scene/renderer3d/lighting/light/spot/SpotLight.h>

namespace urchin {

    SpotLight::SpotLight(const Point3<float>& position, const Vector3<float>& direction, float innerCutOffDegree, float outerCutOffDegree) :
            Light(),
            position(position),
            exponentialAttenuation(0.1f),
            coneScope(nullptr),
            bboxScope(nullptr) {
        directions.emplace_back(direction);

        //TODO assert innerCutOffDegree <= outerCutOffDegree && innerCutOffDegree < 90

        computeScope();
    }

    void SpotLight::setPosition(const Point3<float>& position) {
        this->position = position;

        computeScope();

        notifyObservers(this, Light::LIGHT_MOVE);
    }

    const Point3<float>& SpotLight::getPosition() const {
        return position;
    }

    void SpotLight::setDirection(const Vector3<float>& direction) {
        this->directions.clear();
        this->directions.emplace_back(direction);

        notifyObservers(this, Light::LIGHT_MOVE);
    }

    const std::vector<Vector3<float>>& SpotLight::getDirections() const {
        return directions;
    }

    Light::LightType SpotLight::getLightType() const {
        return Light::LightType::SPOT;
    }

    const AABBox<float>& SpotLight::getAABBox() const {
        return *bboxScope;
    }

    void SpotLight::setAttenuation(float exponentialAttenuation) {
        if (exponentialAttenuation < std::numeric_limits<float>::epsilon()) {
            throw std::domain_error("Exponential attenuation must be greater than zero.");
        }
        this->exponentialAttenuation = exponentialAttenuation;

        computeScope();
    }

    float SpotLight::getExponentialAttenuation() const {
        return exponentialAttenuation;
    }

    const AABBox<float>& SpotLight::getAABBoxScope() const {
        assert(bboxScope);
        return *bboxScope;
    }

    const Cone<float>& SpotLight::getConeScope() const { //TODO use it !
        assert(coneScope);
        return *coneScope;
    }

    /**
     * Computes the sphere scope representing light affectation zone
     */
    void SpotLight::computeScope() { //TODO review !
        float radius = -std::log(ATTENUATION_NO_EFFECT) / getExponentialAttenuation();
        sphereScope = std::make_unique<Sphere<float>>(radius, getPosition());
        bboxScope = std::make_unique<AABBox<float>>(getPosition()-radius, getPosition()+radius);

        notifyOctreeableMove();
    }

}
