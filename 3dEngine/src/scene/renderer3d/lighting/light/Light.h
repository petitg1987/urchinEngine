#pragma once

#include <vector>
#include <UrchinCommon.h>

namespace urchin {

    class Light : public Octreeable<Light> {
        public:
            static constexpr float ATTENUATION_NO_EFFECT = 0.015f; //defines when the attenuation of a light has no light effect on objects

            Light();
            ~Light() override = default;

            enum class LightType {
                SUN = 0,
                OMNIDIRECTIONAL = 1,
                SPOT = 2
            };

            enum NotificationType {
                LIGHT_MOVE, //A light has been moved (position, direction...)
                PRODUCE_SHADOW, //Produce shadow flag has been updated
            };

            //general methods
            virtual const std::vector<Vector3<float>>& getDirections() const = 0;
            virtual const Point3<float>& getPosition() const = 0;
            virtual LightType getLightType() const = 0;
            std::string getName() const override;
            const AABBox<float>& getAABBox() const override = 0;
            const Transform<float>& getTransform() const override;

            //shadow methods
            void setProduceShadow(bool);
            bool isProduceShadow() const;

            //properties methods
            void setLightColor(const Point3<float>&);
            const Point3<float>& getLightColor() const;

        private:
            //general properties
            Transform<float> noTransform; //no transformation because light position is specified in global space
            Point3<float> lightColor;

            //shadow properties
            bool produceShadow;
    };

}
