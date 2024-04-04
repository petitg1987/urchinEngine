#pragma once

namespace urchin {

    class SplitFrustum {
        public:
            explicit SplitFrustum(const Frustum<float>&);

            const Frustum<float>& getFrustum() const;

            const Point3<float>& getFrustumCenter() const;
            float getBoundingSphereRadius() const;

        private:
            Frustum<float> frustum;

            Point3<float> frustumCenter;
            float frustumRadius;
    };

}
