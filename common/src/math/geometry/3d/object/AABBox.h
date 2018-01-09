#ifndef URCHINENGINE_AABBOX_H
#define URCHINENGINE_AABBOX_H

#include <vector>
#include <limits>

#include "math/geometry/3d/object/ConvexObject3D.h"
#include "math/geometry/3d/shape/BoxShape.h"
#include "math/geometry/3d/Ray.h"
#include "math/algebra/matrix/Matrix4.h"
#include "math/algebra/point/Point3.h"
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
			explicit AABBox(const std::vector<Point3<T>> &);
			AABBox(const AABBox<T> &);

			const T getHalfSize(unsigned int) const;
			const Vector3<T> &getHalfSizes() const;
			const T getMaxHalfSize() const;
			unsigned int getMaxHalfSizeIndex() const;
			const T getMinHalfSize() const;
			unsigned int getMinHalfSizeIndex() const;
			const Point3<T> &getMin() const;
			const Point3<T> &getMax() const;
			Point3<T> getCenterOfMass() const;

			Point3<T> getSupportPoint(const Vector3<T> &) const;
			const std::vector<Point3<T>> getPoints() const;
			T getVolume() const;

			AABBox<T> moveAABBox(const Transform<T> &) const;
			Matrix4<T> toProjectionMatrix() const;
			bool include(const AABBox<T> &) const;
			AABBox<T> merge(const AABBox<T> &) const;
			AABBox<T> cutTo(const AABBox<T> &) const;
			AABBox<T> enlarge(const Vector3<T> &, const Vector3<T> &) const;
			AABBox<T> enlarge(T, T) const;

			bool collideWithPoint(const Point3<T> &) const;
			bool collideWithAABBox(const AABBox<T> &) const;
			bool collideWithRay(const Ray<T> &) const;

			const Point3<T>& operator [](std::size_t) const;
			Point3<T>& operator [](std::size_t);

		private:
			BoxShape<T> boxShape;
			Point3<T> min, max;
	};

	template<class T> AABBox<T> operator *(const Matrix4<T> &, const AABBox<T> &);
	template<class T> AABBox<T> operator *(const AABBox<T> &, const Matrix4<T> &);

	template<class T> std::ostream& operator <<(std::ostream &, const AABBox<T> &);

}

 #endif
