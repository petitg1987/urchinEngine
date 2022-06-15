#include <scene/renderer3d/lighting/light/Light.h>

namespace urchin {

    Light::Light() :
            lightColor(Point3<float>(1.0f, 1.0f, 1.0f)),
            produceShadow(false) {

    }

    const Transform<float>& Light::getTransform() const {
        return noTransform;
    }

    void Light::setProduceShadow(bool produceShadow) {
        this->produceShadow = produceShadow;

        notifyObservers(this, Light::PRODUCE_SHADOW);
    }

    bool Light::isProduceShadow() const {
        return produceShadow;
    }

    void Light::setLightColor(const Point3<float>& lightColor) {
        this->lightColor = lightColor;
    }

    const Point3<float>& Light::getLightColor() const {
        return lightColor;
    }

}
