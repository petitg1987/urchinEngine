#ifndef ENGINE_QUATERNION_H
#define ENGINE_QUATERNION_H

#include <iostream>
#include <cmath>
#include <cassert>

#include "math/algebra/vector/Vector3.h"
#include "math/algebra/point/Point3.h"
#include "math/algebra/matrix/Matrix3.h"
#include "math/algebra/matrix/Matrix4.h"

namespace urchin
{
	
	template<class T> class Quaternion
	{
		public:
			Quaternion();
			explicit Quaternion(T Xu, T Yu, T Zu, T Wu);
			explicit Quaternion(const Matrix3<T> &);
			explicit Quaternion(const Vector3<T> &, T);
			explicit Quaternion(const Vector3<T> &);

			void computeW();
			void setIdentity();

			Quaternion<T> normalize() const;
			Quaternion<T> conjugate() const;
			T length() const;
			T squareLength() const;
			T dotProduct(const Quaternion<T> &) const;
			Point3<T> rotatePoint(const Point3<T> &) const;
			Quaternion<T> slerp(const Quaternion<T> &, T t) const;
		
			Matrix4<T> toMatrix4() const;
			Matrix3<T> toMatrix3() const;
			void toAxisAngle(Vector3<T> &, T &) const;
		
			Quaternion<T> operator *(const Quaternion<T> &) const;
			const Quaternion<T>& operator *=(const Quaternion<T> &);
			Quaternion<T> operator *(const Point3<T> &) const;
			const Quaternion<T>& operator *=(const Point3<T> &);
			
			T X, Y, Z, W;
	};

	template<class T> std::ostream& operator <<(std::ostream &, const Quaternion<T> &);

}

#endif
