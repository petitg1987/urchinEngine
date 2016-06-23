#ifndef ENGINE_POINT3_H
#define ENGINE_POINT3_H

#include <iostream>
#include <cmath>

#include "math/algebra/point/Point2.h"
#include "math/algebra/vector/Vector2.h"
#include "math/algebra/vector/Vector3.h"
#include "math/algebra/matrix/Matrix3.h"

namespace urchin
{

	template<class T> class Point3
	{
		public:
			Point3();
			explicit Point3(T Xu, T Yu, T Zu);
			explicit Point3(const Point2<T> &, T Zu=1);
			explicit Point3(const Vector2<T> &, T Zu=1);
			explicit Point3(const Vector3<T> &);
			Point3(const Point3<T> &);

			void setValues(T, T, T);
			void setNull();

			T distance() const;
			T distance(const Point3<T> &) const;
			T squareDistance(const Point3<T> &) const;
			Vector3<T> vector(const Point3<T> &) const;
			Vector3<T> toVector() const;
			Point3<T> translate(const Vector3<T> &) const;
			Point3<T> divideByZ() const;
			Point2<T> toPoint2() const;

			Point3<T> operator +() const;
			Point3<T> operator -() const;
			Point3<T> operator +(const Point3<T> &) const;
			Point3<T> operator -(const Point3<T> &) const;
			const Point3<T>& operator +=(const Point3<T>& v);
			const Point3<T>& operator -=(const Point3<T>& v);
			const Point3<T>& operator *=(T t);
			const Point3<T>& operator /=(T t);
			const Point3<T>& operator +=(T t);
			const Point3<T>& operator -=(T t);

			bool operator ==(const Point3<T> &) const;
			bool operator !=(const Point3<T> &) const;
			bool operator <(const Point3<T> &) const;

			T& operator [](int i);
			const T& operator [](int i) const;

			operator T*();
			operator const T*() const;

			template<class NEW_TYPE> Point3<NEW_TYPE> cast() const;

			T X, Y, Z;
	};

	template<class T> Point3<T> operator *(const Point3<T> &, T t);
	template<class T> Point3<T> operator *(T t, const Point3<T> &);
	template<class T> Point3<T> operator /(const Point3<T> &, T t);
	template<class T> Point3<T> operator +(const Point3<T> &, T t);
	template<class T> Point3<T> operator -(const Point3<T> &, T t);

	template<class T> Point3<T> operator *(const Matrix3<T> &, const Point3<T> &);
	template<class T> Point3<T> operator *(const Point3<T> &, const Matrix3<T> &);

	template<class T> std::ostream& operator <<(std::ostream &, const Point3<T> &);

}

#endif
