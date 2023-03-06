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
            float computeInnerAngle() const;
            float getInnerCutOff() const;
            float computeOuterAngle() const;
            float getOuterCutOff() const;
            LightType getLightType() const override;
            const AABBox<float>& getAABBox() const override;

            //attenuation methods
            void setAttenuation(float);
            float getExponentialAttenuation() const;
            const AABBox<float>& getAABBoxScope() const;
            const Cone<float>& getConeScope() const;

        private:
            void computeScope();

            static constexpr float ATTENUATION_NO_EFFECT = 0.1f; //defines when the attenuation of a light has no light effect on objects

            //general properties
            Point3<float> position;
            std::vector<Vector3<float>> directions;
            float innerCutOff;
            float outerCutOff;

            //attenuation properties
            float exponentialAttenuation;
            std::unique_ptr<Cone<float>> coneScope;
            std::unique_ptr<AABBox<float>> bboxScope;
    };

}
