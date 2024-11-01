#pragma once

#include <iostream>
#include <iomanip>

#include <math/algebra/matrix/Matrix3.h>

namespace urchin {

    /**
    * Represents a matrix 4x4 in column major
    */
    template<class T> class Matrix4 {
        public:
            Matrix4();
            explicit Matrix4(T m11, T m12, T m13, T m14,
                    T m21, T m22, T m23, T m24,
                    T m31, T m32, T m33, T m34,
                    T m41, T m42, T m43, T m44);
            explicit Matrix4(const Matrix3<T>&);

            [[nodiscard]] static Matrix4<T> buildTranslation(T, T, T);
            [[nodiscard]] static Matrix4<T> buildScale(T, T, T);
            [[nodiscard]] static Matrix4<T> buildRotationX(T);
            [[nodiscard]] static Matrix4<T> buildRotationY(T);
            [[nodiscard]] static Matrix4<T> buildRotationZ(T);

            void setValues(T m11, T m12, T m13, T m14,
                    T m21, T m22, T m23, T m24,
                    T m31, T m32, T m33, T m34,
                    T m41, T m42, T m43, T m44);

            void setIdentity();
            T determinant() const;
            [[nodiscard]] Matrix4<T> transpose() const;
            [[nodiscard]] Matrix4<T> inverse() const;
            Matrix3<T> toMatrix3() const;
            [[nodiscard]] Matrix4<T> scaled(T, T, T, T) const;

            Matrix4<T> operator +() const;
            Matrix4<T> operator -() const;
            Matrix4<T> operator +(const Matrix4<T>&) const;
            Matrix4<T> operator -(const Matrix4<T>&) const;
            const Matrix4<T>& operator +=(const Matrix4<T>&);
            const Matrix4<T>& operator -=(const Matrix4<T>&);
            Matrix4<T> operator *(const Matrix4<T>&) const;
            const Matrix4<T>& operator *=(const Matrix4<T>&);
            const Matrix4<T>& operator *=(T);
            const Matrix4<T>& operator /=(T);

            T& operator ()(std::size_t, std::size_t);
            const T& operator ()(std::size_t, std::size_t) const;
            T& operator ()(std::size_t);
            const T& operator ()(std::size_t) const;

            explicit operator T*();
            explicit operator const T*() const;

            T a11;
            T a21;
            T a31;
            T a41;
            T a12;
            T a22;
            T a32;
            T a42;
            T a13;
            T a23;
            T a33;
            T a43;
            T a14;
            T a24;
            T a34;
            T a44;
    };

    template<class T> Matrix4<T> operator *(const Matrix4<T>&, T);
    template<class T> Matrix4<T> operator *(T, const Matrix4<T>&);
    template<class T> Matrix4<T> operator /(const Matrix4<T>&, T);

    template<class T> std::ostream& operator <<(std::ostream&, const Matrix4<T>&);

}
