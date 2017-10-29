#include "object/TemporalObject.h"

namespace urchin
{

	TemporalObject::TemporalObject(const CollisionShape3D *shape, const PhysicsTransform &from, const PhysicsTransform &to) :
		shape(shape),
		localObject(shape->toConvexObject(PhysicsTransform())),
		from(from),
		to(to)
	{

	}

	const CollisionShape3D *TemporalObject::getShape() const
	{
		return shape;
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



