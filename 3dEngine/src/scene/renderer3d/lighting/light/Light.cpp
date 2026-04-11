#include "scene/renderer3d/lighting/light/Light.h"

namespace urchin {

    Light::Light() :
            lightColor(Point3(1.0f, 1.0f, 1.0f)),
            pbrEnabled(true),
            lightMask(std::numeric_limits<uint8_t>::max()),
            produceShadow(false),
            shadowStrength(1.0f) {

    }

    std::string Light::getName() const {
        Vector3<float> dirOrPos = getDirections()[0];
        if (getLightType() != LightType::SUN) {
            dirOrPos = getPosition().toVector();
        }
        return std::format("{:.2f}", dirOrPos.X) + "@" + std::format("{:.2f}",dirOrPos.Y) + "@" + std::format("{:.2f}",dirOrPos.Z) + "_" + std::to_string((int)getLightType());
    }

    const Transform<float>& Light::getTransform() const {
        return noTransform;
    }

    void Light::setLightColor(const Point3<float>& lightColor) {
        this->lightColor = lightColor;
    }

    const Point3<float>& Light::getLightColor() const {
        return lightColor;
    }

    void Light::setPbrEnabled(bool pbrEnabled) {
        this->pbrEnabled = pbrEnabled;
    }

    bool Light::isPbrEnabled() const {
        return pbrEnabled;
    }

    void Light::setLightMask(uint8_t lightMask) {
        this->lightMask = lightMask;
    }

    uint8_t Light::getLightMask() const {
        return lightMask;
    }

    void Light::setProduceShadow(bool produceShadow) {
        if (this->produceShadow != produceShadow) {
            this->produceShadow = produceShadow;

            notifyObservers(this, PRODUCE_SHADOW);
        }
    }

    bool Light::isProduceShadow() const {
        return produceShadow;
    }

    void Light::setShadowStrength(float shadowStrength) {
        this->shadowStrength = shadowStrength;
    }

    float Light::getShadowStrength() const {
        return shadowStrength;
    }

}
