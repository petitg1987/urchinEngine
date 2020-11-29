#ifndef URCHINENGINE_SUNLIGHT_H
#define URCHINENGINE_SUNLIGHT_H

#include "UrchinCommon.h"

#include "scene/renderer3d/lighting/light/Light.h"

namespace urchin {

    class SunLight : public Light {
        public:
            explicit SunLight(const Vector3<float>&);
            ~SunLight() override = default;

            //general methods
            void setDirection(const Vector3<float>&);
            const std::vector<Vector3<float>> &getDirections() const override;
            const Point3<float> &getPosition() const override;
            LightType getLightType() const override;
            bool hasParallelBeams() const override;
            const AABBox<float> &getAABBox() const override;

        private:
            std::vector<Vector3<float>> directions;
    };

}

#endif
