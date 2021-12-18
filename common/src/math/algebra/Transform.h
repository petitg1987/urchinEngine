#pragma once

#include <math/algebra/Quaternion.h>
#include <math/algebra/point/Point3.h>
#include <math/algebra/matrix/Matrix4.h>

namespace urchin {

    /**
    * Class allow to create transform matrix from a given position, orientation and scale
    */
    template<class T> class Transform {
        public:
            Transform();
            explicit Transform(const Point3<T>&, const Quaternion<T> &q = Quaternion<T>(), const Vector3<T>& scale = Vector3<T>(1.0, 1.0, 1.0));

            void setPosition(const Point3<T>&);
            const Point3<T>& getPosition() const;

            void setOrientation(const Quaternion<T>&);
            const Quaternion<T>& getOrientation() const;

            void setScale(const Vector3<T>&);
            const Vector3<T>& getScale() const;
            bool hasScaling() const;

            const Matrix4<T>& getPositionMatrix() const;
            const Matrix4<T>& getOrientationMatrix() const;
            const Matrix4<T>& getScaleMatrix() const;
            const Matrix4<T>& getTransformMatrix() const;

            Transform<T> operator *(const Transform<T>&) const;
            const Transform<T>& operator *=(const Transform<T>&);
            bool operator ==(const Transform<T>&) const;

        private:
            Point3<T> pPosition;
            Quaternion<T> qOrientation;
            Vector3<T> vScale;

            Matrix4<T> mPosition, mOrientation, mScale, mTransform;
    };

}
