#ifndef ENGINE_TRIANGLESHAPE3D_H
#define ENGINE_TRIANGLESHAPE3D_H

#include "math/geometry/3d/shape/ConvexShape3D.h"
#include "math/algebra/point/Point3.h"

namespace urchin
{

	template<class T> class TriangleShape3D : public ConvexShape3D
	{
		public:
			TriangleShape3D(const Point3<T> *);
			TriangleShape3D(const Point3<T> &, const Point3<T> &, const Point3<T> &);

			const Point3<T> *getPoints() const;

		private:
			Point3<T> points[3];
	};

}

#endif
