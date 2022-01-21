#pragma once

#include <iostream>
#include <cmath>

#include <math/algebra/vector/Vector2.h>
#include <math/algebra/matrix/Matrix2.h>

namespace urchin {

    template<class T> class Point2 {
        public:
            Point2();
            Point2(T Xu, T Yu);
            explicit Point2(const Vector2<T>&);
            Point2(const Point2<T>&) = default;
            Point2<T>& operator=(const Point2<T>&) = default;

            void setValues(T, T);
            void setNull();

            T distance(const Point2<T>&) const;
            T squareDistance(const Point2<T>&) const;
            Vector2<T> vector(const Point2<T>&) const;
            Vector2<T> toVector() const;
            [[nodiscard]] Point2<T> translate(const Vector2<T>&) const;

            Point2<T> operator +() const;
            Point2<T> operator -() const;
            Point2<T> operator +(const Point2<T>&) const;
            Point2<T> operator -(const Point2<T>&) const;
            const Point2<T>& operator +=(const Point2<T>& v);
            const Point2<T>& operator -=(const Point2<T>& v);
            const Point2<T>& operator *=(T t);
            const Point2<T>& operator /=(T t);
            const Point2<T>& operator +=(T t);
            const Point2<T>& operator -=(T t);

            bool operator ==(const Point2<T>&) const;
            std::partial_ordering operator <=>(const Point2<T>&) const;

            T& operator [](std::size_t i);
            const T& operator [](std::size_t i) const;

            explicit operator T*();
            explicit operator const T*() const;

            template<class NEW_TYPE> Point2<NEW_TYPE> cast() const;

            T X, Y;
    };

    template<class T> Point2<T> operator *(const Point2<T>&, T t);
    template<class T> Point2<T> operator *(T t, const Point2<T>&);
    template<class T> Point2<T> operator /(const Point2<T>&, T t);
    template<class T> Point2<T> operator +(const Point2<T>&, T t);
    template<class T> Point2<T> operator -(const Point2<T>&, T t);

    template<class T> Point2<T> operator *(const Matrix2<T>&, const Point2<T>&);
    template<class T> Point2<T> operator *(const Point2<T>&, const Matrix2<T>&);

    template<class T> std::ostream& operator <<(std::ostream&, const Point2<T>&);

}
