#pragma once

#include <iostream>
#include <cmath>

#include <math/algebra/vector/Vector2.h>
#include <math/algebra/matrix/Matrix3.h>

namespace urchin {

    template<class T> class Vector3 {
        public:
            Vector3() noexcept;
            Vector3(T Xu, T Yu, T Zu) noexcept;
            explicit Vector3(const Vector2<T>&, T Zu = 0) noexcept;
            Vector3(const Vector3<T>&) = default;
            Vector3<T>& operator=(const Vector3<T>&) = default;

            void setValues(T, T, T);
            void setNull();

            [[nodiscard]] Vector3<T> normalize() const;
            T length() const;
            T squareLength() const;
            T dotProduct(const Vector3<T>&) const;
            Vector3<T> crossProduct(const Vector3<T>&) const;
            Vector3<T> perpendicularVector() const;
            [[nodiscard]] Vector3<T> truncate(T) const;
            Vector2<T> xy() const;
            Vector2<T> xz() const;
            Vector2<T> yz() const;

            Vector3<T> operator +() const;
            Vector3<T> operator -() const;
            Vector3<T> operator +(const Vector3<T>&) const;
            Vector3<T> operator -(const Vector3<T>&) const;
            Vector3<T> operator *(const Vector3<T>&) const;
            Vector3<T> operator /(const Vector3<T>&) const;
            const Vector3<T>& operator +=(const Vector3<T>&);
            const Vector3<T>& operator -=(const Vector3<T>&);
            const Vector3<T>& operator *=(const Vector3<T>&);
            const Vector3<T>& operator /=(const Vector3<T>&);
            const Vector3<T>& operator *=(T t);
            const Vector3<T>& operator /=(T t);

            bool operator ==(const Vector3<T>&) const;
            std::partial_ordering operator <=>(const Vector3<T>&) const;
            bool isEqual(const Vector3<T>&, T) const;

            T& operator [](std::size_t i);
            const T& operator [](std::size_t i) const;

            explicit operator T*();
            explicit operator const T*() const;

            template<class NEW_TYPE> Vector3<NEW_TYPE> cast() const;

            T X;
            T Y;
            T Z;
    };

    template<class T> Vector3<T> operator *(const Vector3<T>&, T t);
    template<class T> Vector3<T> operator *(T t, const Vector3<T>&);
    template<class T> Vector3<T> operator /(const Vector3<T>&, T t);

    template<class T> Vector3<T> operator *(const Matrix3<T>&, const Vector3<T>&);
    template<class T> Vector3<T> operator *(const Vector3<T>&, const Matrix3<T>&);

    template<class T> std::ostream& operator <<(std::ostream&, const Vector3<T>&);

}
