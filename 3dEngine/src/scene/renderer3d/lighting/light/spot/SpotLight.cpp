#include <scene/renderer3d/lighting/light/spot/SpotLight.h>

namespace urchin {

    SpotLight::SpotLight(const Point3<float>& position, const Vector3<float>& direction, float innerAngleInDegrees, float outerAngleInDegrees) :
            position(position),
            innerAngleInDegrees(0.0f),
            innerCosAngle(0.0f),
            outerAngleInDegrees(0.0f),
            outerCosAngle(0.0f),
            aabboxScope(nullptr),
            exponentialAttenuation(0.1f),
            coneScope(nullptr),
            frustumScope(nullptr) {
        directions.emplace_back(direction.normalize());
        setAngles(innerAngleInDegrees, outerAngleInDegrees);

        computeScope();
    }

    void SpotLight::setPosition(const Point3<float>& position) {
        this->position = position;

        computeScope();

        notifyObservers(this, LIGHT_MOVE);
    }

    const Point3<float>& SpotLight::getPosition() const {
        return position;
    }

    void SpotLight::setDirection(const Vector3<float>& direction) {
        if (direction.squareLength() < std::numeric_limits<float>::epsilon()) {
            throw std::invalid_argument("Invalid zero length direction for spot light");
        }
        this->directions.clear();
        this->directions.emplace_back(direction.normalize());

        computeScope();

        notifyObservers(this, LIGHT_MOVE);
    }

    const std::vector<Vector3<float>>& SpotLight::getDirections() const {
        return directions;
    }

    void SpotLight::setAngles(float innerAngleInDegrees, float outerAngleInDegrees) {
        if (innerAngleInDegrees > MAX_ANGLE_DEGREE) {
            throw std::out_of_range("Inner angle for a spot light must be below 89°: " + std::to_string(innerAngleInDegrees));
        } else if (outerAngleInDegrees > MAX_ANGLE_DEGREE) {
            throw std::out_of_range("Outer angle for a spot light must be below 89°: " + std::to_string(outerAngleInDegrees));
        }

        if (innerAngleInDegrees > outerAngleInDegrees) {
            innerAngleInDegrees = outerAngleInDegrees;
        }

        this->innerAngleInDegrees = innerAngleInDegrees;
        this->innerCosAngle = std::cos(AngleConverter<float>::toRadian(innerAngleInDegrees));
        this->outerAngleInDegrees = outerAngleInDegrees;
        this->outerCosAngle = std::cos(AngleConverter<float>::toRadian(outerAngleInDegrees));

        computeScope();
    }

    /**
     * Return the inner angle in degrees between the symmetry line of the cone and a side of the once
     */
    float SpotLight::getInnerAngle() const {
        return innerAngleInDegrees;
    }

    float SpotLight::getInnerCosAngle() const {
        return innerCosAngle;
    }

    /**
     * Return the outer angle in degrees between the symmetry line of the cone and a side of the once
     */
    float SpotLight::getOuterAngle() const {
        return outerAngleInDegrees;
    }

    float SpotLight::getOuterCosAngle() const {
        return outerCosAngle;
    }

    Light::LightType SpotLight::getLightType() const {
        return LightType::SPOT;
    }

    const AABBox<float>& SpotLight::getAABBox() const {
        assert(aabboxScope);
        return *aabboxScope;
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

    float SpotLight::computeIlluminationRange() const {
        return -std::log(ATTENUATION_NO_EFFECT) / exponentialAttenuation;
    }

    const Cone<float>& SpotLight::getConeScope() const {
        assert(coneScope);
        return *coneScope;
    }

    const Frustum<float>& SpotLight::getFrustumScope() const {
        assert(frustumScope);
        return *frustumScope;
    }

    /**
     * Computes the cone scope representing light affectation zone
     */
    void SpotLight::computeScope() {
        float coneHeight = computeIlluminationRange();
        Quaternion<float> orientation = Quaternion<float>::rotationFromTo(Vector3(0.0f, 0.0f, -1.0f), directions[0]).normalize();

        Point3<float> coneCenterOfMass = getPosition().translate(directions[0] * (coneHeight * (3.0f / 4.0f)));
        float coneRadius = coneHeight * std::tan(AngleConverter<float>::toRadian(outerAngleInDegrees));
        coneScope = std::make_unique<Cone<float>>(coneRadius, coneHeight, ConeShape<float>::ConeOrientation::CONE_Z_POSITIVE, coneCenterOfMass, orientation);

        float nearPlane = 0.025f;
        float farPlane = coneHeight + nearPlane;
        Matrix4<float> transformMatrix = Matrix4<float>::buildTranslation(getPosition().X, getPosition().Y, getPosition().Z) * orientation.toMatrix4();
        Frustum frustum(outerAngleInDegrees * 2.0f, 1.0f, nearPlane, farPlane);
        frustumScope = std::make_unique<Frustum<float>>(transformMatrix * frustum);

        float minX = coneScope->getSupportPoint(Vector3(-1.0f, 0.0f, 0.0f)).X;
        float maxX = coneScope->getSupportPoint(Vector3(1.0f, 0.0f, 0.0f)).X;
        float minY = coneScope->getSupportPoint(Vector3(0.0f, -1.0f, 0.0f)).Y;
        float maxY = coneScope->getSupportPoint(Vector3(0.0f, 1.0f, 0.0f)).Y;
        float minZ = coneScope->getSupportPoint(Vector3(0.0f, 0.0f, -1.0f)).Z;
        float maxZ = coneScope->getSupportPoint(Vector3(0.0f, 0.0f, 1.0f)).Z;
        aabboxScope = std::make_unique<AABBox<float>>(Point3(minX, minY, minZ), Point3(maxX, maxY, maxZ));

        notifyOctreeableMove();
    }

    std::unique_ptr<Light> SpotLight::clone() const {
        auto cloned =  std::make_unique<SpotLight>(position, directions[0], innerAngleInDegrees, outerAngleInDegrees);
        cloned->setAttenuation(exponentialAttenuation);
        return cloned;
    }

}
