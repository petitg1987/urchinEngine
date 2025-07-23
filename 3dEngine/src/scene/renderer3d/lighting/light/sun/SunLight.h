#pragma once

#include <UrchinCommon.h>

#include "scene/renderer3d/lighting/light/Light.h"

namespace urchin {

    class SunLight final : public Light {
        public:
            explicit SunLight(const Vector3<float>&);
            SunLight(const SunLight&) = default;
            ~SunLight() override = default;

            //general methods
            void setDirection(const Vector3<float>&);
            const std::vector<Vector3<float>>& getDirections() const override;
            const Point3<float>& getPosition() const override;
            LightType getLightType() const override;
            std::string getLightTypeName() const override;
            const AABBox<float>& getAABBox() const override;

            std::unique_ptr<Light> clone() const override;

        private:
            std::vector<Vector3<float>> directions;
    };

}
