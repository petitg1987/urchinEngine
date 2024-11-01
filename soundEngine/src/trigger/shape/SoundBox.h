#pragma once

#include <UrchinCommon.h>

#include <trigger/shape/SoundShape.h>

namespace urchin {

    /**
    * Box used to delimit the sound
    */
    class SoundBox final : public SoundShape {
        public:
            SoundBox(const Vector3<float>&, const Point3<float>&, const Quaternion<float>&, float);

            ShapeType getShapeType() const override;

            const OBBox<float>& getPlayTriggerBox() const;
            const OBBox<float>& getStopTriggerBox() const;

            const Vector3<float>& getHalfSizes() const;
            const Point3<float>& getCenterPosition() const override;
            void updateCenterPosition(const Point3<float>&) override;
            const Quaternion<float>& getOrientation() const;
            const Vector3<float>& getAxis(unsigned int) const;

            bool pointInsidePlayShape(const Point3<float>&) const override;
            bool pointInsideStopShape(const Point3<float>&) const override;

            std::unique_ptr<SoundShape> clone() const override;

        private:
            bool pointInsideShape(const Point3<float>&, const OBBox<float>&) const;

            OBBox<float> playTriggerBox;
            OBBox<float> stopTriggerBox;
    };

}
