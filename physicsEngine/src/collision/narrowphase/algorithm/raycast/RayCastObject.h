#ifndef ENGINE_RAYCASTOBJECT_H
#define ENGINE_RAYCASTOBJECT_H

#include "shape/CollisionShape3D.h"
#include "utils/math/PhysicsTransform.h"

namespace urchin
{

	class RayCastObject
	{
		public:
			RayCastObject(std::shared_ptr<const CollisionConvexObject3D>, const PhysicsTransform &, const PhysicsTransform &);
			~RayCastObject();

			std::shared_ptr<const CollisionConvexObject3D> getObject() const;

			const PhysicsTransform &getFrom() const;
			const PhysicsTransform &getTo() const;

		private:
			std::shared_ptr<const CollisionConvexObject3D> object;

			PhysicsTransform from;
			PhysicsTransform to;
	};

}

#endif
