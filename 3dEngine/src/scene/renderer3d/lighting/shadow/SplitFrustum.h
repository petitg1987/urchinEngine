#pragma once

namespace urchin {

    class SplitFrustum {
        public:
            explicit SplitFrustum(const Frustum<float>&);

            const Frustum<float>& getFrustum() const;

            const Sphere<float>& getBoundingSphere() const;

        private:
            Frustum<float> frustum;

            Sphere<float> boundingSphere;
    };

}
