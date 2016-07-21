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

			const CollisionShape3D *getShape() const;

			const PhysicsTransform &getFrom() const;
			const PhysicsTransform &getTo() const;

		private:
			const CollisionShape3D *shape;

			PhysicsTransform from;
			PhysicsTransform to;
	};

}

#endif
