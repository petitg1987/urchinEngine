#pragma once

#include <iostream>

#include <math/algebra/matrix/Matrix2.h>

namespace urchin {

    template<class T> class Vector2 {
        public:
            Vector2() noexcept;
            Vector2(T Xu, T Yu) noexcept;

            void setValues(T, T);
            void setNull();

            [[nodiscard]] Vector2<T> normalize() const;
            T length() const;
            T squareLength() const;
            T dotProduct(const Vector2<T>&) const;
            T crossProduct(const Vector2<T>&) const;
            Vector2<T> perpendicularVector() const;
            [[nodiscard]] Vector2<T> truncate(T) const;

            Vector2<T> operator +() const;
            Vector2<T> operator -() const;
            Vector2<T> operator +(const Vector2<T>&) const;
            Vector2<T> operator -(const Vector2<T>&) const;
            Vector2<T> operator *(const Vector2<T>&) const;
            Vector2<T> operator /(const Vector2<T>&) const;
            const Vector2<T>& operator +=(const Vector2<T>&);
            const Vector2<T>& operator -=(const Vector2<T>&);
            const Vector2<T>& operator *=(const Vector2<T>&);
            const Vector2<T>& operator /=(const Vector2<T>&);
            const Vector2<T>& operator *=(T t);
            const Vector2<T>& operator /=(T t);

            bool operator ==(const Vector2<T>&) const;
            std::partial_ordering operator <=>(const Vector2<T>&) const;
            bool isEqual(const Vector2<T>&, T) const;

            T& operator [](std::size_t i);
            const T& operator [](std::size_t i) const;

            explicit operator T*();
            explicit operator const T*() const;

            template<class NEW_TYPE> Vector2<NEW_TYPE> cast() const;

            T X;
            T Y;
    };

    template<class T> Vector2<T> operator *(const Vector2<T>&, T t);
    template<class T> Vector2<T> operator *(T t, const Vector2<T>&);
    template<class T> Vector2<T> operator /(const Vector2<T>&, T t);

    template<class T> Vector2<T> operator *(const Matrix2<T>&, const Vector2<T>&);
    template<class T> Vector2<T> operator *(const Vector2<T>&, const Matrix2<T>&);

    template<class T> std::ostream& operator <<(std::ostream&, const Vector2<T>&);

}
