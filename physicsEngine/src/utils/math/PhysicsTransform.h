#pragma once

#include <UrchinCommon.h>

namespace urchin {

    /**
    * Class allow to create transform matrix from a given position and orientation
    */
    class PhysicsTransform {
        public:
            PhysicsTransform();
            explicit PhysicsTransform(const Point3<float>&, const Quaternion<float> & = Quaternion<float>());
            explicit PhysicsTransform(const Transform<float>&);

            void setPosition(const Point3<float>&);
            const Point3<float>& getPosition() const;

            void setOrientation(const Quaternion<float>&);
            const Quaternion<float>& getOrientation() const;
            Matrix3<float> retrieveOrientationMatrix() const;

            Transform<float> toTransform() const;
            PhysicsTransform inverse() const;

            bool equals(const PhysicsTransform&) const;
            Point3<float> transform(const Point3<float>&) const;
            Point3<float> inverseTransform(const Point3<float>&) const;
            PhysicsTransform integrate(const Vector3<float>&, const Vector3<float>&, float) const;

            PhysicsTransform operator *(const PhysicsTransform&) const;
            const PhysicsTransform& operator *=(const PhysicsTransform&);

        private:
            Point3<float> position;
            Quaternion<float> orientation;
    };

    std::ostream& operator <<(std::ostream&, const PhysicsTransform&);

}
