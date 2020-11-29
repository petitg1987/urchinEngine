#ifndef URCHINENGINE_QUATERNION_H
#define URCHINENGINE_QUATERNION_H

#include <iostream>

#include "math/algebra/vector/Vector3.h"
#include "math/algebra/point/Point3.h"
#include "math/algebra/matrix/Matrix3.h"
#include "math/algebra/matrix/Matrix4.h"

namespace urchin {

    template<class T> class Quaternion {
        public:
          enum RotationSequence {
                XYZ,
                XZY,
                YXZ,
                YZX,
                ZXY,
                ZYX,

                XYX,
                XZX,
                YXY,
                YZY,
                ZXZ,
                ZYZ
            };

            Quaternion();
            explicit Quaternion(T Xu, T Yu, T Zu, T Wu);
            explicit Quaternion(const Matrix3<T>&);
            Quaternion(const Vector3<T>&, T);
            Quaternion(const Vector3<T>&, RotationSequence);
            explicit Quaternion(const Vector3<T>&, const Vector3<T> &normalizedUp = Vector3<T>(0.0, 1.0, 0.0));

            void computeW();
            void setIdentity();

            Quaternion<T> normalize() const;
            Quaternion<T> conjugate() const;
            Quaternion<T> inverse() const;
            T norm() const;
            T squareNorm() const;
            T dotProduct(const Quaternion<T>&) const;
            Point3<T> rotatePoint(const Point3<T>&) const;
            Quaternion<T> slerp(const Quaternion<T>&, T t) const;
            Quaternion<T> lerp(const Quaternion<T>&, T t) const;
            Vector3<T> getForwardDirection() const;

            Matrix4<T> toMatrix4() const;
            Matrix3<T> toMatrix3() const;
            void toAxisAngle(Vector3<T>&, T&) const;
            Vector3<T> toEulerAngle(RotationSequence) const;

            Quaternion<T> operator +(const Quaternion<T>&) const;
            Quaternion<T> operator -(const Quaternion<T>&) const;
            Quaternion<T> operator *(const Quaternion<T>&) const;
            const Quaternion<T>& operator +=(const Quaternion<T>&);
            const Quaternion<T>& operator -=(const Quaternion<T>&);
            const Quaternion<T>& operator *=(const Quaternion<T>&);
            const Quaternion<T>& operator *=(const Point3<T>&);
            const Quaternion<T>& operator *=(T);

            bool operator ==(const Quaternion<T>&) const;
            bool operator !=(const Quaternion<T>&) const;

            T& operator [](std::size_t i);
            const T& operator [](std::size_t i) const;

            T X, Y, Z, W;

        private:
            Vector3<T> threeAxisEulerRotation(int, int, int) const;
            Vector3<T> twoAxisEulerRotation(int, int, int) const;
    };

    template<class T> Quaternion<T> operator *(const Quaternion<T>&, const Point3<T>&);
    template<class T> Quaternion<T> operator *(const Quaternion<T>&, T);
    template<class T> Quaternion<T> operator *(T, const Quaternion<T>&);

    template<class T> std::ostream& operator <<(std::ostream&, const Quaternion<T>&);

}

#endif
