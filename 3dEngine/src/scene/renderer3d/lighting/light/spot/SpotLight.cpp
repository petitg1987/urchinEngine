#include <scene/renderer3d/lighting/light/spot/SpotLight.h>

namespace urchin {

    SpotLight::SpotLight(const Point3<float>& position, const Vector3<float>& direction, float innerAngleDegree, float outerAngleDegree) :
            Light(),
            position(position),
            innerCutOff(0.0f),
            outerCutOff(0.0f),
            exponentialAttenuation(0.1f),
            coneScope(nullptr),
            bboxScope(nullptr) {
        directions.emplace_back(direction.normalize());
        setCutOffDegree(innerAngleDegree, outerAngleDegree);

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

        computeScope();

        notifyObservers(this, Light::LIGHT_MOVE);
    }

    const std::vector<Vector3<float>>& SpotLight::getDirections() const {
        return directions;
    }

    void SpotLight::setCutOffDegree(float innerAngleDegree, float outerAngleDegree) {
        if (innerAngleDegree >= 90.0f) {
            throw std::out_of_range("Inner angle for a spot light must be below 90°: " + std::to_string(innerAngleDegree));
        } else if (outerAngleDegree >= 90.0f) {
            throw std::out_of_range("Outer angle for a spot light must be below 90°: " + std::to_string(outerAngleDegree));
        } else if (outerAngleDegree < innerAngleDegree) {
            throw std::runtime_error("Outer angle (" + std::to_string(outerAngleDegree) + ") must be greater or equal to the inner angle (" + std::to_string(innerAngleDegree) + ") for a spot light");
        }

        this->innerCutOff = std::cos(AngleConverter<float>::toRadian(innerAngleDegree));
        this->outerCutOff = std::cos(AngleConverter<float>::toRadian(outerAngleDegree));

        computeScope();
    }

    float SpotLight::getInnerCutOff() const {
        return innerCutOff;
    }

    float SpotLight::getOuterCutOff() const {
        return outerCutOff;
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

    const Cone<float>& SpotLight::getConeScope() const {
        assert(coneScope);
        return *coneScope;
    }

    /**
     * Computes the cone scope representing light affectation zone
     */
    void SpotLight::computeScope() {
        float radius = -std::log(ATTENUATION_NO_EFFECT) / getExponentialAttenuation();

        Point3<float> coneCenterOfMass = getPosition().translate(directions[0] * (radius * (3.0f / 4.0f)));
        float radiusCone = 5.0f; //TODO compute
        Quaternion<float> orientation = Quaternion<float>::rotationFromTo(Vector3<float>(0.0, -1.0, 0.0), directions[0]);
        coneScope = std::make_unique<Cone<float>>(radiusCone, radius, ConeShape<float>::ConeOrientation::CONE_Y_NEGATIVE, coneCenterOfMass, orientation);

        float halfRadius = radius / 2.0f;
        Point3<float> sphereCenter = getPosition().translate(directions[0] * halfRadius);
        bboxScope = std::make_unique<AABBox<float>>(sphereCenter - halfRadius, sphereCenter + halfRadius);

        notifyOctreeableMove();
    }

}
