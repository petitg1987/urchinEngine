#include <scene/renderer3d/lighting/shadow/SplitFrustum.h>

namespace urchin {

    SplitFrustum::SplitFrustum(const Frustum<float>& frustum) :
            frustum(frustum) {
        Sphere<float> frustumBoundingSphere = frustum.computeBoundingSphere();
        float enlargedRadius = std::ceil(frustumBoundingSphere.getRadius() + 0.05f);
        boundingSphere = Sphere<float>(enlargedRadius, frustumBoundingSphere.getCenterOfMass());
    }

    const Frustum<float>& SplitFrustum::getFrustum() const {
        return frustum;
    }

    const Sphere<float>& SplitFrustum::getBoundingSphere() const {
        return boundingSphere;
    }

}