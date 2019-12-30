#ifndef URCHINENGINE_LINESEGMENT3D_H
#define URCHINENGINE_LINESEGMENT3D_H

#include <cmath>
#include <vector>

#include "math/geometry/3d/object/ConvexObject3D.h"
#include "math/geometry/3d/Line3D.h"
#include "math/algebra/point/Point3.h"
#include "math/algebra/vector/Vector3.h"

namespace urchin
{

	template<class T> class LineSegment3D : public ConvexObject3D<T>
	{
		public:
            LineSegment3D();
			LineSegment3D(const Point3<T> &, const Point3<T> &);

			const Point3<T> &getA() const;
			const Point3<T> &getB() const;

			Point3<T> getSupportPoint(const Vector3<T> &) const;
			Point3<T> closestPoint(const Point3<T> &) const;
			Point3<T> closestPoint(const Point3<T> &, T [2]) const;
			T squareDistance(const Point3<T> &) const;

            Vector3<T> toVector() const;
            Line3D<T> toLine() const;

		private:
			Point3<T> a, b;
	};

	template<class T> std::ostream& operator <<(std::ostream &, const LineSegment3D<T> &);

}

#endif
