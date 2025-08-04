#include "scene/renderer3d/lighting/light/Light.h"

namespace urchin {

    Light::Light() :
            lightColor(Point3(1.0f, 1.0f, 1.0f)),
            pbrEnabled(true),
            lightMask(255),
            produceShadow(false),
            shadowStrength(1.0f) {

    }

    std::string Light::getName() const {
        if (getLightType() == LightType::SUN) {
            return std::to_string(getDirections()[0].X) + "@" + std::to_string(getDirections()[0].Y) + "@" + std::to_string(getDirections()[0].Z) + "_" + std::to_string((int)getLightType());
        }
        return std::to_string(getPosition().X) + "@" + std::to_string(getPosition().Y) + "@" + std::to_string(getPosition().Z) + "_" + std::to_string((int)getLightType());
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
