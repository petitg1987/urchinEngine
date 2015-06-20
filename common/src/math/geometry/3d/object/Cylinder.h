#ifndef ENGINE_CYLINDER_H
#define ENGINE_CYLINDER_H

#include <map>
#include <cmath>

#include "ConvexObject3D.h"
#include "math/geometry/3d/shape/CylinderShape.h"
#include "math/algebra/point/Point3.h"
#include "math/algebra/vector/Vector3.h"
#include "math/algebra/Quaternion.h"

namespace urchin
{

	template<class T> class Cylinder : public ConvexObject3D<T>
	{
		public:
			Cylinder();
			Cylinder(T, T, typename CylinderShape<T>::CylinderOrientation, const Point3<T> &, const Quaternion<T> &);
			~Cylinder();

			T getRadius() const;
			T getHeight() const;
			typename CylinderShape<T>::CylinderOrientation getCylinderOrientation() const;
			const Point3<T> &getCenterPosition() const;
			const Quaternion<T> &getOrientation() const;
			const Vector3<T> &getAxis(unsigned int) const;

			Point3<T> getSupportPoint(const Vector3<T> &) const;

		private:
			CylinderShape<T> cylinderShape;
			Point3<T> centerPosition;
			Quaternion<T> orientation;

			Vector3<T> axis[3]; //3 vectors of normalized axis
	};

}

 #endif
