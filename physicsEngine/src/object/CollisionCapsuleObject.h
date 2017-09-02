#ifndef URCHINENGINE_COLLISIONCAPSULEOBJECT_H
#define URCHINENGINE_COLLISIONCAPSULEOBJECT_H

#include <string>
#include "UrchinCommon.h"

#include "object/CollisionConvexObject3D.h"

namespace urchin
{

	class CollisionCapsuleObject : public CollisionConvexObject3D
	{
		public:
			CollisionCapsuleObject(float, float, float, typename CapsuleShape<float>::CapsuleOrientation, const Point3<float> &, const Quaternion<float> &);
			~CollisionCapsuleObject();

			float getRadius() const;
			float getCylinderHeight() const;
			typename CapsuleShape<float>::CapsuleOrientation getCapsuleOrientation() const;
			const Point3<float> &getCenterOfMass() const;
			const Quaternion<float> &getOrientation() const;
			const Vector3<float> &getAxis(unsigned int) const;

			Point3<float> getSupportPoint(const Vector3<float> &, bool) const;

			const Capsule<float> retrieveCapsule() const;
			std::string toString() const;

		private:
			const Capsule<float> capsuleObject; //object without margin
	};

}

#endif
