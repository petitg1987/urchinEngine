#pragma once

#include <vector>
#include <UrchinCommon.h>

#include <scene/renderer3d/lighting/light/Light.h>

namespace urchin {

    class OmnidirectionalLight final : public Light {
        public:
            static constexpr float FRUSTUM_NEAR_PLANE = 0.025f;

            explicit OmnidirectionalLight(const Point3<float>&);
            OmnidirectionalLight(const OmnidirectionalLight&) = default;

            //general methods
            void setPosition(const Point3<float>&);
            const Point3<float>& getPosition() const override;
            const std::vector<Vector3<float>>& getDirections() const override;
            LightType getLightType() const override;
            std::string getLightTypeName() const override;
            const AABBox<float>& getAABBox() const override;

            //attenuation methods
            void setAttenuation(float);
            float getExponentialAttenuation() const;
            float computeRadius() const;
            const AABBox<float>& getAABBoxScope() const;
            const Sphere<float>& getSphereScope() const;
            const Frustum<float>& getFrustumScope(unsigned int) const;

            std::unique_ptr<Light> clone() const override;

        private:
            void computeScope();

            //general properties
            Point3<float> position;
            std::vector<Vector3<float>> directions;

            //attenuation properties
            float exponentialAttenuation;
            std::optional<Sphere<float>> sphereScope;
            std::optional<AABBox<float>> bboxScope;
            std::optional<std::array<Frustum<float>, 6>> frustumScopes;
    };

}
