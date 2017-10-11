#ifndef URCHINENGINE_POINT4_H
#define URCHINENGINE_POINT4_H

#include <iostream>
#include <cmath>

#include "math/algebra/point/Point2.h"
#include "math/algebra/point/Point3.h"
#include "math/algebra/vector/Vector3.h"
#include "math/algebra/vector/Vector4.h"
#include "math/algebra/matrix/Matrix4.h"

namespace urchin
{

	template<class T> class Point4
	{
		public:
			Point4();
			explicit Point4(T Xu, T Yu, T Zu, T Wu);
			explicit Point4(const Point2<T> &, T Zu=0, T Wu=1);
			explicit Point4(const Point3<T> &, T Wu=1);
			explicit Point4(const Vector3<T> &, T wu=1);
			explicit Point4(const Vector4<T> &);
			Point4(const Point4<T> &);

			void setValues(T, T, T, T);
			void setNull();

			T distance() const;
			T distance(const Point4<T> &) const;
			T squareDistance(const Point4<T> &) const;
			Vector4<T> vector(const Point4<T> &) const;
			Vector4<T> toVector() const;
			Point4<T> translate(const Vector4<T> &) const;
			Point4<T> divideByW() const;
			Point3<T> toPoint3() const;

			Point4<T> operator +() const;
			Point4<T> operator -() const;
			Point4<T> operator +(const Point4<T> &) const;
			Point4<T> operator -(const Point4<T> &) const;
			const Point4<T>& operator +=(const Point4<T>& v);
			const Point4<T>& operator -=(const Point4<T>& v);
			const Point4<T>& operator *=(T t);
			const Point4<T>& operator /=(T t);
			const Point4<T>& operator +=(T t);
			const Point4<T>& operator -=(T t);

			bool operator ==(const Point4<T> &) const;
			bool operator !=(const Point4<T> &) const;
			bool operator <(const Point4<T> &) const;

			T& operator [](int i);
			const T& operator [](int i) const;

			explicit operator T*();
			explicit operator const T*() const;

			template<class NEW_TYPE> Point4<NEW_TYPE> cast() const;

			T X, Y, Z, W;
	};

	template<class T> Point4<T> operator *(const Point4<T> &, T t);
	template<class T> Point4<T> operator *(T t, const Point4<T> &);
	template<class T> Point4<T> operator /(const Point4<T> &, T t);
	template<class T> Point4<T> operator +(const Point4<T> &, T t);
	template<class T> Point4<T> operator -(const Point4<T> &, T t);

	template<class T> Point4<T> operator *(const Matrix4<T> &, const Point4<T> &);
	template<class T> Point4<T> operator *(const Point4<T> &, const Matrix4<T> &);

	template<class T> std::ostream& operator <<(std::ostream &, const Point4<T> &);

}

#endif
