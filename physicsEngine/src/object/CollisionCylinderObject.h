#ifndef ENGINE_COLLISIONCYLINDEROBJECT_H
#define ENGINE_COLLISIONCYLINDEROBJECT_H

#include "UrchinCommon.h"

#include "object/CollisionConvexObject3D.h"

namespace urchin
{

	class CollisionCylinderObject : public CollisionConvexObject3D
	{
		public:
			CollisionCylinderObject(float, float, float, typename CylinderShape<float>::CylinderOrientation, const Point3<float> &, const Quaternion<float> &);
			~CollisionCylinderObject();

			float getRadius() const;
			float getHeight() const;
			typename CylinderShape<float>::CylinderOrientation getCylinderOrientation() const;
			const Point3<float> &getCenterPosition() const;
			const Quaternion<float> &getOrientation() const;
			const Vector3<float> &getAxis(unsigned int) const;

			Point3<float> getSupportPoint(const Vector3<float> &, bool) const;

			const Cylinder<float> retrieveCylinder() const;

		private:
			const Cylinder<float> cylinderObject; //object without margin
	};

}

#endif
