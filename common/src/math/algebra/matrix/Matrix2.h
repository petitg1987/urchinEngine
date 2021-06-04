#pragma once

#include <iostream>
#include <cmath>
#include <iomanip>

namespace urchin {

    /**
    * Represents a matrix 2x2 in column major
    */
    template<class T> class Matrix2 {
        public:
            Matrix2();
            explicit Matrix2(T m11, T m12,
                    T m21, T m22);

            void setValues(T m11, T m12,
                    T m21, T m22);

            void setIdentity();
            T determinant() const;
            Matrix2<T> transpose() const;
            Matrix2<T> inverse() const;
            Matrix2<T> scaled(T, T) const;

            void buildScale(T, T);

            Matrix2<T> operator +() const;
            Matrix2<T> operator -() const;
            Matrix2<T> operator +(const Matrix2<T>&) const;
            Matrix2<T> operator -(const Matrix2<T>&) const;
            const Matrix2<T>& operator +=(const Matrix2<T>&);
            const Matrix2<T>& operator -=(const Matrix2<T>&);
            Matrix2<T> operator *(const Matrix2<T>&) const;
            const Matrix2<T>& operator *=(const Matrix2<T>&);
            const Matrix2<T>& operator *=(T);
            const Matrix2<T>& operator /=(T);

            T& operator ()(std::size_t, std::size_t);
            const T& operator ()(std::size_t, std::size_t) const;
            T& operator ()(std::size_t);
            const T& operator ()(std::size_t) const;

            explicit operator T*();
            explicit operator const T*() const;

            T   a11, a21,
                a12, a22;
    };

    template<class T> Matrix2<T> operator *(const Matrix2<T>&, T);
    template<class T> Matrix2<T> operator *(T, const Matrix2<T>&);
    template<class T> Matrix2<T> operator /(const Matrix2<T>&, T);

    template<class T> std::ostream& operator <<(std::ostream&, const Matrix2<T>&);

}
