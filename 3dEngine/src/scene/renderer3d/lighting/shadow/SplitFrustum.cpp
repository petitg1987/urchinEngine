#include <scene/renderer3d/lighting/shadow/SplitFrustum.h>

namespace urchin {

    SplitFrustum::SplitFrustum(const Frustum<float>& frustum) :
            frustum(frustum),
            frustumCenter(frustum.computeBoundingSphereCenter()),
            frustumRadius(0.0f) {
        for (const Point3<float>& frustumPoint : frustum.getFrustumPoints()) {
            frustumRadius = std::max(frustumRadius, frustumCenter.squareDistance(frustumPoint));
        }
        frustumRadius = std::ceil(std::sqrt(frustumRadius));
    }

    const Frustum<float>& SplitFrustum::getFrustum() const {
        return frustum;
    }

    const Point3<float>& SplitFrustum::getFrustumCenter() const {
        return frustumCenter;
    }

    float SplitFrustum::getBoundingSphereRadius() const {
        return frustumRadius;
    }

}