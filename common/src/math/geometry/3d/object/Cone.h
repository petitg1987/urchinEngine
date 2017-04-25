#ifndef ENGINE_CONE_H
#define ENGINE_CONE_H

#include <cmath>

#include "math/geometry/3d/object/ConvexObject3D.h"
#include "math/geometry/3d/shape/ConeShape.h"
#include "math/algebra/point/Point3.h"
#include "math/algebra/vector/Vector3.h"
#include "math/algebra/Quaternion.h"

namespace urchin
{

	template<class T> class Cone : public ConvexObject3D<T>
	{
		public:
			Cone();
			Cone(T, T, typename ConeShape<T>::ConeOrientation, const Point3<T> &, const Quaternion<T> &);
			~Cone();

			T getRadius() const;
			T getHeight() const;
			typename ConeShape<T>::ConeOrientation getConeOrientation() const;
			const Point3<T> &getCenterOfMass() const;
			const Quaternion<T> &getOrientation() const;
			const Vector3<T> &getAxis(unsigned int) const;

			Point3<T> getCenter() const;
			Point3<T> getSupportPoint(const Vector3<T> &) const;

		private:
			ConeShape<T> coneShape;
			Point3<T> centerOfMass;
			Quaternion<T> orientation;

			Vector3<T> axis[3]; //3 vectors of normalized axis
			T baseSideCosAngle;
	};

}

#endif
