#ifndef URCHINENGINE_PLANE_H
#define URCHINENGINE_PLANE_H

#include "Line3D.h"
#include "math/algebra/point/Point3.h"
#include "math/algebra/point/Point4.h"
#include "math/algebra/vector/Vector3.h"

namespace urchin
{
	
	template<class T> class Plane
	{
		public:
			Plane(const Vector3<T> &, T);
			Plane(const Point3<T> &, const Point3<T> &, const Point3<T> &);
			Plane(const Vector3<T> &, const Point3<T> &);
			explicit Plane(T a=0.0, T b=1.0, T c=0.0, T d=0.0);
			Plane(const Plane<T> &);
            Plane<T> operator=(const Plane<T> &);
			Plane<T> operator=(Plane<T> &&);
		
			void buildFrom3Points(const Point3<T> &, const Point3<T> &, const Point3<T> &);
			void buildFromNormalAndPoint(const Vector3<T> &, const Point3<T> &);
			void buildFromCoefficients(T, T, T, T);
			void reverse();
		
			const Vector3<T> &getNormal() const;
			void setNormal(const Vector3<T> &);
			T getDistanceToOrigin() const;
			void setDistanceToOrigin(T);
		
			T distance(const Point3<T> &) const;
			T distance(const Point4<T> &) const;
			T verticalDistance(const Point3<T> &) const;
			T horizontalDistance(const Point3<T> &) const;
			Point3<T> orthogonalProjection(const Point3<T> &) const;

			Point3<T> intersectPoint(const Line3D<T> &) const;
		
		private:
			Vector3<T> normal; //plane normal normalized
			T d; //Distance to the origin. Positive if dot product between a vector from plane to origin and the normal is positive
	};

	template<class T> std::ostream& operator <<(std::ostream &, const Plane<T> &);

}

#endif
