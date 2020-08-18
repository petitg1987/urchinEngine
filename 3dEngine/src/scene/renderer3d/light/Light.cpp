#include "Light.h"

namespace urchin
{

    Light::Light() :
            ambientColor(Point3<float>(0.0f, 0.0f, 0.0f)),
            produceShadow(false)
    {

    }

    const Transform<float> &Light::getTransform() const
    {
        return noTransform;
    }

    void Light::setProduceShadow(bool produceShadow)
    {
        this->produceShadow = produceShadow;

        notifyObservers(this, Light::PRODUCE_SHADOW);
    }

    bool Light::isProduceShadow() const
    {
        return produceShadow;
    }

    void Light::setAmbientColor(const Point3<float> &ambientColor)
    {
        this->ambientColor = ambientColor;
    }

    const Point3<float> &Light::getAmbientColor() const
    {
        return ambientColor;
    }

}
