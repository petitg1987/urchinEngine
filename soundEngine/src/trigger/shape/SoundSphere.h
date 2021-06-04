#pragma once

#include <UrchinCommon.h>

#include <trigger/shape/SoundShape.h>

namespace urchin {

    /**
    * Sphere used to delimit the sound
    */
    class SoundSphere : public SoundShape {
        public:
            SoundSphere(float, const Point3<float>&, float);

            SoundShape::ShapeType getShapeType() const override;

            const Sphere<float>& getPlayTriggerSphere() const;
            const Sphere<float>& getStopTriggerSphere() const;

            float getRadius() const;
            Point3<float> getPosition() const;

            bool pointInsidePlayShape(const Point3<float>&) const override;
            bool pointInsideStopShape(const Point3<float>&) const override;

            SoundShape* clone() const override;

        private:
            const Sphere<float> playTriggerSphere;
            const Sphere<float> stopTriggerSphere;
    };

}
