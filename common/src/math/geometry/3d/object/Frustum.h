#ifndef URCHINENGINE_FRUSTUM_H
#define URCHINENGINE_FRUSTUM_H

#include <vector>

#include "math/geometry/3d/object/ConvexObject3D.h"
#include "math/geometry/3d/object/Sphere.h"
#include "math/geometry/3d/object/AABBox.h"
#include "math/geometry/3d/Plane.h"
#include "math/algebra/matrix/Matrix4.h"
#include "math/algebra/vector/Vector3.h"
#include "math/algebra/point/Point3.h"

namespace urchin
{
	
	/**
	* Represents a frustum (truncated pyramid) in space
	*/
	template<class T> class Frustum : public ConvexObject3D<T>
	{
		public:
			Frustum();
			Frustum(T, T, T, T);
			Frustum(const Point3<T> &, const Point3<T> &, const Point3<T> &, const Point3<T> &, const Point3<T> &, const Point3<T> &, const Point3<T> &, const Point3<T> &);
			~Frustum();

			enum FrustumPoint
			{
				NTL = 0, //Near top left
				NTR, //Near top right
				NBL, //Near bottom left
				NBR, //Near bottom right
				FTL, //Far top left
				FTR, //Far top right
				FBL, //Far bottom left
				FBR //Far bottom right
			};

			void buildFrustum(T, T, T, T);
			void buildFrustum(const Point3<T> &, const Point3<T> &, const Point3<T> &, const Point3<T> &, const Point3<T> &, const Point3<T> &, const Point3<T> &, const Point3<T> &);

			const std::vector<Point3<T>> &getFrustumPoints() const;
			const Point3<T> &getFrustumPoint(FrustumPoint frustumPoint) const;
			const Point3<T> &getPosition() const;
			
			Point3<T> getSupportPoint(const Vector3<T> &) const;
			T computeNearDistance() const;
			T computeFarDistance() const;
			Frustum<T> splitFrustum(T, T) const;

			bool collideWithPoint(const Point3<T> &) const;
			bool collideWithAABBox(const AABBox<T> &) const;
			bool collideWithSphere(const Sphere<T> &) const;

		private:
			void buildData();

			std::vector<Point3<T>> frustumPoints;
			Point3<T> position; //eye/camera position
			Plane<T> planes[6];
			enum {
				TOP = 0,
				BOTTOM,
				LEFT,
				RIGHT,
				NEARP,
				FARP
			};
	};

	template<class T> Frustum<T> operator *(const Matrix4<T> &, const Frustum<T> &);
	template<class T> Frustum<T> operator *(const Frustum<T> &, const Matrix4<T> &);

	template<class T> std::ostream& operator <<(std::ostream &, const Frustum<T> &);

}

#endif
