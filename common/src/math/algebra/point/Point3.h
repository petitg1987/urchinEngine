#pragma once

#include <ostream>

#include "math/algebra/point/Point2.h"
#include "math/algebra/vector/Vector2.h"
#include "math/algebra/vector/Vector3.h"
#include "math/algebra/matrix/Matrix3.h"

namespace urchin {

    template<class T> class Point3 {
        public:
            struct Hash {
                std::size_t operator()(const Point3<T>&) const;
            };

            Point3() noexcept;
            Point3(T Xu, T Yu, T Zu) noexcept;
            explicit Point3(const Point2<T>&, T Zu = 1) noexcept;
            explicit Point3(const Vector2<T>&, T Zu = 1) noexcept;
            explicit Point3(const Vector3<T>&);
            Point3(const Point3<T>&) = default;
            Point3<T>& operator=(const Point3<T>&) = default;

            void setValues(T, T, T);
            void setNull();

            T distance(const Point3<T>&) const;
            T squareDistance(const Point3<T>&) const;
            Vector3<T> vector(const Point3<T>&) const;
            Vector3<T> toVector() const;
            [[nodiscard]] Point3<T> translate(const Vector3<T>&) const;
            Point3<T> divideByZ() const;
            Point2<T> toPoint2XY() const;
            Point2<T> toPoint2XZ() const;
            Point2<T> toPoint2YZ() const;

            Point3<T> operator +() const;
            Point3<T> operator -() const;
            Point3<T> operator +(const Point3<T>&) const;
            Point3<T> operator -(const Point3<T>&) const;
            const Point3<T>& operator +=(const Point3<T>&);
            const Point3<T>& operator -=(const Point3<T>&);
            const Point3<T>& operator *=(T t);
            const Point3<T>& operator /=(T t);
            const Point3<T>& operator +=(T t);
            const Point3<T>& operator -=(T t);

            bool operator ==(const Point3<T>&) const;
            std::partial_ordering operator <=>(const Point3<T>&) const;
            bool isEqual(const Point3<T>&, T) const;

            T& operator [](std::size_t i);
            const T& operator [](std::size_t i) const;

            explicit operator T*();
            explicit operator const T*() const;

            template<class NEW_TYPE> Point3<NEW_TYPE> cast() const;

            T X;
            T Y;
            T Z;
    };

    template<class T> Point3<T> operator *(const Point3<T>&, T t);
    template<class T> Point3<T> operator *(T t, const Point3<T>&);
    template<class T> Point3<T> operator /(const Point3<T>&, T t);
    template<class T> Point3<T> operator +(const Point3<T>&, T t);
    template<class T> Point3<T> operator -(const Point3<T>&, T t);

    template<class T> Point3<T> operator *(const Matrix3<T>&, const Point3<T>&);
    template<class T> Point3<T> operator *(const Point3<T>&, const Matrix3<T>&);

    template<class T> std::ostream& operator <<(std::ostream&, const Point3<T>&);

}
