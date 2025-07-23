#pragma once

#include <vector>
#include <UrchinCommon.h>

#include "scene/renderer3d/lighting/light/Light.h"

namespace urchin {

    class SpotLight final : public Light {
        public:
            static constexpr float MAX_ANGLE_DEGREE = 80.0f;
            static constexpr float FRUSTUM_NEAR_PLANE = 0.01f;

            explicit SpotLight(const Point3<float>&, const Vector3<float>&, float, float);
            SpotLight(const SpotLight&) = default;

            //general methods
            void setPosition(const Point3<float>&);
            const Point3<float>& getPosition() const override;
            void setDirection(const Vector3<float>&);
            const std::vector<Vector3<float>>& getDirections() const override;
            void setAngles(float, float);
            float getInnerAngle() const;
            float getInnerCosAngle() const;
            float getOuterAngle() const;
            float getOuterCosAngle() const;
            LightType getLightType() const override;
            std::string getLightTypeName() const override;
            const AABBox<float>& getAABBox() const override;

            //attenuation methods
            void setAttenuation(float);
            float getExponentialAttenuation() const;
            float computeIlluminationRange() const;
            float computeEndRadius() const;
            const Cone<float>& getConeScope() const;
            const Frustum<float>& getFrustumScope() const;

            std::unique_ptr<Light> clone() const override;

        private:
            void computeScope();

            //general properties
            Point3<float> position;
            std::vector<Vector3<float>> directions;
            float innerAngleInDegrees;
            float innerCosAngle;
            float outerAngleInDegrees;
            float outerCosAngle;
            std::optional<AABBox<float>> aabboxScope;

            //attenuation properties
            float exponentialAttenuation;
            std::optional<Cone<float>> coneScope;
            std::optional<Frustum<float>> frustumScope;
    };

}
