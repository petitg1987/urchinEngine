#ifndef ENGINE_TRIANGLE3D_H
#define ENGINE_TRIANGLE3D_H

#include "ConvexObject3D.h"
#include "math/geometry/3d/shape/Triangle3DShape.h"
#include "math/algebra/point/Point3.h"
#include "math/algebra/vector/Vector3.h"

namespace urchin
{

	template<class T> class Triangle3D : public ConvexObject3D<T>
	{
		public:
			Triangle3D(const Point3<T> *);
			Triangle3D(const Point3<T> &, const Point3<T> &, const Point3<T> &);

			const Point3<T> *getPoints() const;

			Point3<T> getSupportPoint(const Vector3<T> &) const;
			Vector3<T> computeNormal() const;
			Point3<T> closestPoint(const Point3<T> &, T [3]) const;

		private:
			Triangle3DShape<T> triangleShape;
	};

}

#endif
