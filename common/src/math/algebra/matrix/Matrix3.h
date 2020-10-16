#ifndef URCHINENGINE_MATRIX3_H
#define URCHINENGINE_MATRIX3_H

#include <iostream>
#include <cmath>
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

            void setValues(T m11, T m12, T m13,
                    T m21, T m22, T m23,
                    T m31, T m32, T m33);

            void setIdentity();
            T determinant() const;
            Matrix3<T> transpose() const;
            Matrix3<T> inverse() const;
            Matrix3<T> scaled(T, T, T) const;

            void buildTranslation(T, T);
            void buildScale(T, T, T);
            void buildRotationX(T);
            void buildRotationY(T);
            void buildRotationZ(T);

            Matrix3<T> operator +() const;
            Matrix3<T> operator -() const;
            Matrix3<T> operator +(const Matrix3<T>&) const;
            Matrix3<T> operator -(const Matrix3<T>&) const;
            const Matrix3<T>& operator +=(const Matrix3<T>&);
            const Matrix3<T>& operator -=(const Matrix3<T>&);
            Matrix3<T> operator *(const Matrix3<T> &) const;
            const Matrix3<T>& operator *=(const Matrix3<T>&);
            const Matrix3<T>& operator *=(T);
            const Matrix3<T>& operator /=(T);

            T& operator ()(std::size_t, std::size_t);
            const T& operator ()(std::size_t, std::size_t) const;
            T& operator ()(std::size_t);
            const T& operator ()(std::size_t) const;

            explicit operator T*();
            explicit operator const T*() const;

            T     a11, a21, a31,
                a12, a22, a32,
                a13, a23, a33;
    };

    template<class T> Matrix3<T> operator *(const Matrix3<T> &, T);
    template<class T> Matrix3<T> operator *(T, const Matrix3<T> &);
    template<class T> Matrix3<T> operator /(const Matrix3<T> &, T);

    template<class T> std::ostream& operator <<(std::ostream &, const Matrix3<T> &);

}

#endif
