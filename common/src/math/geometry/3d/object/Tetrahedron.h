#ifndef ENGINE_TETRAHEDRON_H
#define ENGINE_TETRAHEDRON_H

#include <limits>

#include "ConvexObject3D.h"
#include "math/algebra/point/Point3.h"
#include "math/algebra/vector/Vector3.h"

namespace urchin
{

	/**
	* Represents a tetrahedron
	*/
	template<class T> class Tetrahedron : public ConvexObject3D<T>
	{
		public:
			Tetrahedron(const Point3<T> &, const Point3<T> &, const Point3<T> &, const Point3<T> &);

			Point3<T> getSupportPoint(const Vector3<T> &) const;
			Point3<T> closestPoint(const Point3<T> &, T [4], short voronoiRegionMask = 15) const;

			bool collideWithPoint(const Point3<T> &) const;

		private:
			bool pointOutsideOrInPlane(const Point3<T> &, const Point3<T> &, const Point3<T> &, const Point3<T> &, const Point3<T> &) const;

			Point3<T> a, b, c, d;
	};

}

#endif
