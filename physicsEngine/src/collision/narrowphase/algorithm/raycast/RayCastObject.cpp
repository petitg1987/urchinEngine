#include "collision/narrowphase/algorithm/raycast/RayCastObject.h"

namespace urchin
{

	RayCastObject::RayCastObject(std::shared_ptr<const CollisionConvexObject3D> object, const PhysicsTransform &from, const PhysicsTransform &to) :
		object(object),
		from(from),
		to(to)
	{

	}

	RayCastObject::~RayCastObject()
	{

	}

	std::shared_ptr<const CollisionConvexObject3D> RayCastObject::getObject() const
	{
		return object;
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



