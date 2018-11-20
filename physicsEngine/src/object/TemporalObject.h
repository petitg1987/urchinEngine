#ifndef URCHINENGINE_TEMPORALOBJECT_H
#define URCHINENGINE_TEMPORALOBJECT_H

#include "shape/CollisionShape3D.h"
#include "utils/math/PhysicsTransform.h"

namespace urchin
{

	class TemporalObject
	{
		public:
			TemporalObject(const CollisionShape3D *, const PhysicsTransform &, const PhysicsTransform &);

			const CollisionShape3D *getShape() const;
			const CollisionConvexObject3D *getLocalObject() const;

			const PhysicsTransform &getFrom() const;
			const PhysicsTransform &getTo() const;

		private:
			const CollisionShape3D *shape;
			const CollisionConvexObject3D *localObject;

			PhysicsTransform from;
			PhysicsTransform to;
	};

}

#endif
