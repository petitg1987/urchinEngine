#include "collision/narrowphase/algorithm/raycast/RayCastObject.h"

namespace urchin
{

	RayCastObject::RayCastObject(const CollisionShape3D *shape, const PhysicsTransform &from, const PhysicsTransform &to) :
		localObject(shape->toConvexObject(PhysicsTransform())),
		from(from),
		to(to)
	{

	}

	RayCastObject::~RayCastObject()
	{

	}

	std::shared_ptr<const CollisionConvexObject3D> RayCastObject::getLocalObject() const
	{
		return localObject;
	}

	const PhysicsTransform &RayCastObject::getFrom() const
	{
		return from;
	}

	const PhysicsTransform &RayCastObject::getTo() const
	{
		return to;
	}

}



