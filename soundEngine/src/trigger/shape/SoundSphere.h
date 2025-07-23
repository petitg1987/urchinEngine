#pragma once

#include <UrchinCommon.h>

#include "trigger/shape/SoundShape.h"

namespace urchin {

    /**
    * Sphere used to delimit the sound
    */
    class SoundSphere final : public SoundShape {
        public:
            SoundSphere(float, const Point3<float>&, float);

            ShapeType getShapeType() const override;

            const Sphere<float>& getPlayTriggerSphere() const;
            const Sphere<float>& getStopTriggerSphere() const;

            float getRadius() const;
            const Point3<float>& getCenterPosition() const override;
            void updateCenterPosition(const Point3<float>&) override;

            bool pointInsidePlayShape(const Point3<float>&) const override;
            bool pointInsideStopShape(const Point3<float>&) const override;

            std::unique_ptr<SoundShape> clone() const override;

        private:
            Sphere<float> playTriggerSphere;
            Sphere<float> stopTriggerSphere;
    };

}
