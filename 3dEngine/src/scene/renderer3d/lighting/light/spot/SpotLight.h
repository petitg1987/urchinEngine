#pragma once

#include <vector>
#include <UrchinCommon.h>

#include <scene/renderer3d/lighting/light/Light.h>

namespace urchin {

    class SpotLight final : public Light {
        public:
            static constexpr float MAX_ANGLE_DEGREE = 89.0f;

            explicit SpotLight(const Point3<float>&, const Vector3<float>&, float, float);

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
            const AABBox<float>& getAABBox() const override;

            //attenuation methods
            void setAttenuation(float);
            float getExponentialAttenuation() const;
            const AABBox<float>& getAABBoxScope() const;
            const Cone<float>& getConeScope() const;

        private:
            void computeScope();

            //general properties
            Point3<float> position;
            std::vector<Vector3<float>> directions;
            float innerAngleInDegrees;
            float innerCosAngle;
            float outerAngleInDegrees;
            float outerCosAngle;

            //attenuation properties
            float exponentialAttenuation;
            std::unique_ptr<Cone<float>> coneScope;
            std::unique_ptr<AABBox<float>> bboxScope;
    };

}
