#ifndef URCHINENGINE_VECTOR4_H
#define URCHINENGINE_VECTOR4_H

#include <iostream>
#include <cmath>

#include "math/algebra/vector/Vector3.h"
#include "math/algebra/matrix/Matrix4.h"

namespace urchin {

    template<class T> class Vector4 {
        public:
            Vector4();
            explicit Vector4(T Xu, T Yu, T Zu, T Wu);
            explicit Vector4(const Vector3<T> &, T Wu=1);
            Vector4(const Vector4<T> &);
            Vector4<T>& operator=(const Vector4<T> &);

            void setValues(T, T, T, T);
            void setNull();

            Vector4<T> normalize() const;
            T length() const;
            T squareLength() const;
            T dotProduct(const Vector4<T> &) const;
            Vector3<T> xyz() const;

            Vector4<T> operator +() const;
            Vector4<T> operator -() const;
            Vector4<T> operator +(const Vector4<T> &) const;
            Vector4<T> operator -(const Vector4<T> &) const;
            Vector4<T> operator *(const Vector4<T> &) const;
            Vector4<T> operator /(const Vector4<T> &) const;
            const Vector4<T>& operator +=(const Vector4<T>&);
            const Vector4<T>& operator -=(const Vector4<T>&);
            const Vector4<T>& operator *=(const Vector4<T> &);
            const Vector4<T>& operator /=(const Vector4<T> &);
            const Vector4<T>& operator *=(T t);
            const Vector4<T>& operator /=(T t);

            T& operator [](int i);
            const T& operator [](int i) const;

            explicit operator T*();
            explicit operator const T*() const;

            template<class NEW_TYPE> Vector4<NEW_TYPE> cast() const;

            T X, Y, Z, W;
    };

    template<class T> Vector4<T> operator *(const Vector4<T> &, T t);
    template<class T> Vector4<T> operator *(T t, const Vector4<T> &);
    template<class T> Vector4<T> operator /(const Vector4<T> &, T t);

    template<class T> Vector4<T> operator *(const Matrix4<T> &, const Vector4<T> &);
    template<class T> Vector4<T> operator *(const Vector4<T> &, const Matrix4<T> &);

    template<class T> std::ostream& operator <<(std::ostream &, const Vector4<T> &);

}

#endif
