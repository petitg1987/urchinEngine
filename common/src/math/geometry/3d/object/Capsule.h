#ifndef ENGINE_CAPSULE_H
#define ENGINE_CAPSULE_H

#include <map>
#include <cmath>

#include "math/geometry/3d/object/ConvexObject3D.h"
#include "math/geometry/3d/shape/CapsuleShape.h"
#include "math/algebra/point/Point3.h"
#include "math/algebra/vector/Vector3.h"
#include "math/algebra/Quaternion.h"

namespace urchin
{

	template<class T> class Capsule : public ConvexObject3D<T>
	{
		public:
			Capsule();
			Capsule(T, T, typename CapsuleShape<T>::CapsuleOrientation, const Point3<T> &, const Quaternion<T> &);
			~Capsule();

			T getRadius() const;
			T getCylinderHeight() const;
			typename CapsuleShape<T>::CapsuleOrientation getCapsuleOrientation() const;
			const Point3<T> &getCenterPosition() const;
			const Quaternion<T> &getOrientation() const;
			const Vector3<T> &getAxis(unsigned int) const;

			Point3<T> getSupportPoint(const Vector3<T> &) const;

		private:
			CapsuleShape<T> capsuleShape;
			Point3<T> centerPosition;
			Quaternion<T> orientation;

			Vector3<T> axis[3]; //3 vectors of normalized axis
	};

	template<class T> std::ostream& operator <<(std::ostream &, const Capsule<T> &);

}

 #endif
