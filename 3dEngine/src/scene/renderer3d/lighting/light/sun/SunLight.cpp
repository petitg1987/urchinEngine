#include <stdexcept>

#include <scene/renderer3d/lighting/light/sun/SunLight.h>

namespace urchin {

    SunLight::SunLight(const Vector3<float>& direction) {
        this->directions.emplace_back(direction);
    }

    void SunLight::setDirection(const Vector3<float>& direction) {
        this->directions.clear();
        this->directions.emplace_back(direction);

        notifyObservers(this, LIGHT_MOVE);
    }

    const std::vector<Vector3<float>>& SunLight::getDirections() const {
        return directions;
    }

    const Point3<float>& SunLight::getPosition() const {
        throw std::runtime_error("No position for sun light.");
    }

    Light::LightType SunLight::getLightType() const {
        return LightType::SUN;
    }

    const AABBox<float>& SunLight::getAABBox() const {
        throw std::runtime_error("No bounding box for sun light.");
    }

}
