#ifndef ENGINE_AABBOX_H
#define ENGINE_AABBOX_H

#include <vector>
#include <limits>

#include "ConvexObject3D.h"
#include "math/geometry/3d/shape/BoxShape.h"
#include "math/algebra/matrix/Matrix4.h"
#include "math/algebra/point/Point3.h"
#include "math/algebra/point/Point4.h"
#include "math/algebra/vector/Vector3.h"
#include "math/algebra/Transform.h"

namespace urchin
{

	/**
	* Represents an axis aligned bounding box
	*/
	template<class T> class AABBox : public ConvexObject3D<T>
	{
		public:
			AABBox();
			AABBox(const Point3<T> &, const Point3<T> &);
			AABBox(const Point3<T> &, const Vector3<T>&);
			~AABBox();

			const T getHalfSize(unsigned int) const;
			const Vector3<T> &getHalfSizes() const;
			const T getMaxHalfSize() const;
			const Point3<T> &getMin() const;
			const Point3<T> &getMax() const;

			Point3<T> getSupportPoint(const Vector3<T> &) const;
			AABBox<T> moveAABBox(const Transform<T> &) const;
			Matrix4<T> toProjectionMatrix() const;
			AABBox<T> merge(const AABBox<T> &) const;
			AABBox<T> cutTo(const AABBox<T> &) const;
			const std::vector<Point3<T>> getEightPoints() const;

			bool collideWithPoint(const Point3<T> &) const;
			bool collideWithAABBox(const AABBox<T> &) const;

		private:
			BoxShape<T> boxShape;
			Point3<T> min, max;
	};

	template<class T> AABBox<T> operator *(const Matrix4<T> &, const AABBox<T> &);
	template<class T> AABBox<T> operator *(const AABBox<T> &, const Matrix4<T> &);

	template<class T> std::ostream& operator <<(std::ostream &, const AABBox<T> &);

}

 #endif
