#ifndef ENGINE_RAYCASTOBJECT_H
#define ENGINE_RAYCASTOBJECT_H

#include "shape/CollisionShape3D.h"
#include "utils/math/PhysicsTransform.h"

namespace urchin
{

	class RayCastObject
	{
		public:
			RayCastObject(const CollisionShape3D *, const PhysicsTransform &, const PhysicsTransform &);
			~RayCastObject();

			std::shared_ptr<const CollisionConvexObject3D> getLocalObject() const;

			const PhysicsTransform &getFrom() const;
			const PhysicsTransform &getTo() const;

		private:
			std::shared_ptr<const CollisionConvexObject3D> localObject;

			PhysicsTransform from;
			PhysicsTransform to;
	};

}

#endif
