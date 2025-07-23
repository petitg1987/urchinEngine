#pragma once

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
            Quaternion(T Xu, T Yu, T Zu, T Wu);
            static Quaternion<T> fromRotationMatrix(const Matrix3<T>&);
            static Quaternion<T> fromEuler(const Vector3<T>&, RotationSequence);
            static Quaternion<T> lookUp(const Vector3<T>&, const Vector3<T> &normalizedUp = Vector3<T>(0.0, 1.0, 0.0));
            static Quaternion<T> rotationFromTo(const Vector3<T>&, const Vector3<T> &);
            static Quaternion<T> fromAxisAngle(const Vector3<T>&, T);
            static Quaternion<T> rotationX(T);
            static Quaternion<T> rotationY(T);
            static Quaternion<T> rotationZ(T);

            void computeW();
            void setIdentity();

            [[nodiscard]] Quaternion<T> normalize() const;
            [[nodiscard]] Quaternion<T> conjugate() const;
            [[nodiscard]] Quaternion<T> inverse() const;
            T norm() const;
            T squareNorm() const;
            T dotProduct(const Quaternion<T>&) const;
            Point3<T> rotatePoint(const Point3<T>&) const;
            Vector3<T> rotateVector(const Vector3<T>&) const;
            [[nodiscard]] Quaternion<T> slerp(const Quaternion<T>&, T t) const;
            [[nodiscard]] Quaternion<T> lerp(const Quaternion<T>&, T t) const;
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
            const Quaternion<T>& operator *=(const Vector3<T>&);
            const Quaternion<T>& operator *=(T);

            bool operator ==(const Quaternion<T>&) const;
            bool isEqualOrientation(const Quaternion<T>&, T) const;
            bool isEqualRotation(const Quaternion<T>&, T) const;

            T& operator [](std::size_t i);
            const T& operator [](std::size_t i) const;

            T X;
            T Y;
            T Z;
            T W;

        private:
            Vector3<T> threeAxisEulerRotation(std::size_t, std::size_t, std::size_t) const;
            Vector3<T> twoAxisEulerRotation(std::size_t, std::size_t, std::size_t) const;
    };

    template<class T> Quaternion<T> operator *(const Quaternion<T>&, const Point3<T>&);
    template<class T> Quaternion<T> operator *(const Quaternion<T>&, const Vector3<T>&);
    template<class T> Quaternion<T> operator *(const Quaternion<T>&, T);
    template<class T> Quaternion<T> operator *(T, const Quaternion<T>&);

    template<class T> std::ostream& operator <<(std::ostream&, const Quaternion<T>&);

}
