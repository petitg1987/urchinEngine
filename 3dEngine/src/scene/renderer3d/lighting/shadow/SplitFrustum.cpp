#include <scene/renderer3d/lighting/shadow/SplitFrustum.h>

namespace urchin {

    SplitFrustum::SplitFrustum(const Frustum<float>& frustum) :
            frustum(frustum),
            boundingSphere(frustum.computeBoundingSphere()) { //TODO ceil round the radius ?

    }

    const Frustum<float>& SplitFrustum::getFrustum() const {
        return frustum;
    }

    const Sphere<float>& SplitFrustum::getBoundingSphere() const {
        return boundingSphere;
    }

}