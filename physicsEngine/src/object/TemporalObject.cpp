#include "object/TemporalObject.h"

namespace urchin
{

	TemporalObject::TemporalObject(const CollisionShape3D *shape, const PhysicsTransform &from, const PhysicsTransform &to) :
		localObject(shape->toConvexObject(PhysicsTransform())),
		from(from),
		to(to)
	{

	}

	TemporalObject::~TemporalObject()
	{

	}

	std::shared_ptr<const CollisionConvexObject3D> TemporalObject::getLocalObject() const
	{
		return localObject;
	}

	const PhysicsTransform &TemporalObject::getFrom() const
	{
		return from;
	}

	const PhysicsTransform &TemporalObject::getTo() const
	{
		return to;
	}

}



