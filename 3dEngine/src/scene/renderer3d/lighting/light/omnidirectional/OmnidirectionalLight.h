#pragma once

#include <vector>
#include <UrchinCommon.h>

#include <scene/renderer3d/lighting/light/Light.h>

namespace urchin {

    class OmnidirectionalLight final : public Light {
        public:
            explicit OmnidirectionalLight(const Point3<float>&);

            //general methods
            void setPosition(const Point3<float>&);
            const Point3<float>& getPosition() const override;
            const std::vector<Vector3<float>>& getDirections() const override;
            LightType getLightType() const override;
            const AABBox<float>& getAABBox() const override;

            //attenuation methods
            void setAttenuation(float);
            float getExponentialAttenuation() const;
            const AABBox<float>& getAABBoxScope() const;
            const Sphere<float>& getSphereScope() const;

            std::unique_ptr<Light> clone() const override;

        private:
            void computeScope();

            //general properties
            Point3<float> position;
            std::vector<Vector3<float>> directions;

            //attenuation properties
            float exponentialAttenuation;
            std::unique_ptr<Sphere<float>> sphereScope;
            std::unique_ptr<AABBox<float>> bboxScope;
    };

}
