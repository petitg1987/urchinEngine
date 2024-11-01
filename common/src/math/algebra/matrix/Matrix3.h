#pragma once

#include <iostream>
#include <iomanip>

namespace urchin {
    /**
    * Represents a matrix 3x3 in column major
    */
    template<class T> class Matrix3 {
        public:
            Matrix3();
            explicit Matrix3(T m11, T m12, T m13,
                    T m21, T m22, T m23,
                    T m31, T m32, T m33);

            [[nodiscard]] static Matrix3<T> buildTranslation(T, T);
            [[nodiscard]] static Matrix3<T> buildScale(T, T, T);
            [[nodiscard]] static Matrix3<T> buildRotationX(T);
            [[nodiscard]] static Matrix3<T> buildRotationY(T);
            [[nodiscard]] static Matrix3<T> buildRotationZ(T);

            void setValues(T m11, T m12, T m13,
                    T m21, T m22, T m23,
                    T m31, T m32, T m33);

            void setIdentity();
            T determinant() const;
            [[nodiscard]] Matrix3<T> transpose() const;
            [[nodiscard]] Matrix3<T> inverse() const;
            [[nodiscard]] Matrix3<T> scaled(T, T, T) const;

            Matrix3<T> operator +() const;
            Matrix3<T> operator -() const;
            Matrix3<T> operator +(const Matrix3<T>&) const;
            Matrix3<T> operator -(const Matrix3<T>&) const;
            const Matrix3<T>& operator +=(const Matrix3<T>&);
            const Matrix3<T>& operator -=(const Matrix3<T>&);
            Matrix3<T> operator *(const Matrix3<T>&) const;
            const Matrix3<T>& operator *=(const Matrix3<T>&);
            const Matrix3<T>& operator *=(T);
            const Matrix3<T>& operator /=(T);

            T& operator ()(std::size_t, std::size_t);
            const T& operator ()(std::size_t, std::size_t) const;
            T& operator ()(std::size_t);
            const T& operator ()(std::size_t) const;

            explicit operator T*();
            explicit operator const T*() const;

            T a11;
            T a21;
            T a31;
            T a12;
            T a22;
            T a32;
            T a13;
            T a23;
            T a33;
    };

    template<class T> Matrix3<T> operator *(const Matrix3<T>&, T);
    template<class T> Matrix3<T> operator *(T, const Matrix3<T>&);
    template<class T> Matrix3<T> operator /(const Matrix3<T>&, T);

    template<class T> std::ostream& operator <<(std::ostream&, const Matrix3<T>&);

}
