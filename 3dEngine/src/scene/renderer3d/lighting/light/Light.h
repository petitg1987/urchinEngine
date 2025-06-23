#pragma once

#include <vector>
#include <UrchinCommon.h>

namespace urchin {

    class Light : public Octreeable<Light> {
        public:
            static constexpr float ATTENUATION_NO_EFFECT = 0.015f; //defines when the attenuation of a light has no light effect on objects
            static constexpr int LIGHT_FLAG_PRODUCE_SHADOW = 0x01; //must match with _lightingFunctions.frag
            static constexpr int LIGHT_FLAG_PBR_ENABLED = 0x02; //must match with _lightingFunctions.frag

            Light();
            Light(const Light&) = default;
            ~Light() override = default;

            enum class LightType {
                SUN = 0,
                OMNIDIRECTIONAL = 1,
                SPOT = 2
            };

            enum NotificationType {
                AFFECTED_ZONE_UPDATED, //Light affectation zone is updated (position, direction, spot angle...)
                PRODUCE_SHADOW, //Produce shadow flag has been updated
                ILLUMINATED_AREA_SIZE_UPDATED, //Light illuminated area size has been updated
            };

            virtual const std::vector<Vector3<float>>& getDirections() const = 0;
            virtual const Point3<float>& getPosition() const = 0;
            virtual LightType getLightType() const = 0;
            virtual std::string getLightTypeName() const = 0;
            std::string getName() const override;
            const AABBox<float>& getAABBox() const override = 0;
            const Transform<float>& getTransform() const override;

            void setLightColor(const Point3<float>&);
            const Point3<float>& getLightColor() const;

            void setPbrEnabled(bool);
            bool isPbrEnabled() const;

            void setProduceShadow(bool);
            bool isProduceShadow() const;

            virtual std::unique_ptr<Light> clone() const = 0;

        private:
            Transform<float> noTransform; //no transformation because light position is specified in global space
            Point3<float> lightColor;
            bool pbrEnabled;
            bool produceShadow;
    };

}
