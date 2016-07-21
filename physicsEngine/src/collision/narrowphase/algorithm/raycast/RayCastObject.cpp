#include "collision/narrowphase/algorithm/raycast/RayCastObject.h"

namespace urchin
{

	RayCastObject::RayCastObject(const CollisionShape3D *shape, const PhysicsTransform &from, const PhysicsTransform &to) :
		shape(shape),
		from(from),
		to(to)
	{

	}

	RayCastObject::~RayCastObject()
	{

	}

	const CollisionShape3D *RayCastObject::getShape() const
	{
		return shape;
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



