#include "Fog.h"

namespace urchin {

    Fog::Fog(float density, float gradient, const Vector3<float>& color, float maxHeight) :
            density(density),
            gradient(gradient),
            color(color),
            maxHeight(maxHeight) {

    }

    float Fog::getDensity() const {
        return density;
    }

    float Fog::getGradient() const {
        return gradient;
    }

    const Vector3<float>& Fog::getColor() {
        return color;
    }

    float Fog::getMaxHeight() const {
        return maxHeight;
    }
}
