#ifndef ENGINE_TRIANGLE3DSHAPE_H
#define ENGINE_TRIANGLE3DSHAPE_H

#include "Shape3D.h"
#include "math/algebra/point/Point3.h"
#include "math/algebra/vector/Vector3.h"

namespace urchin
{

	template<class T> class Triangle3DShape : public Shape3D
	{
		public:
			Triangle3DShape(const Point3<T> *);
			Triangle3DShape(const Point3<T> &, const Point3<T> &, const Point3<T> &);

			const Point3<T> *getPoints() const;

		private:
			Point3<T> points[3];
	};

}

#endif
