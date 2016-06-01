#ifndef ENGINE_COLLISIONBOXOBJECT_H
#define ENGINE_COLLISIONBOXOBJECT_H

#include <sstream>
#include "UrchinCommon.h"

#include "object/CollisionConvexObject3D.h"

namespace urchin
{

	class CollisionBoxObject : public CollisionConvexObject3D
	{
		public:
			CollisionBoxObject(float, const Vector3<float> &, const Point3<float> &, const Quaternion<float> &);
			~CollisionBoxObject();

			float getHalfSize(unsigned int) const;
			Vector3<float> getHalfSizes() const;
			const Point3<float> &getCenterPosition() const;
			const Quaternion<float> &getOrientation() const;
			const Vector3<float> &getAxis(unsigned int) const;

			Point3<float> getSupportPoint(const Vector3<float> &, bool) const;

			const OBBox<float> retrieveOBBox() const;
			std::string toString() const;

		private:
			const OBBox<float> boxObject; //object without margin
	};

}

#endif
