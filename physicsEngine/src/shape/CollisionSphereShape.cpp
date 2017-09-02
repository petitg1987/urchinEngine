#include "shape/CollisionSphereShape.h"
#include "object/CollisionSphereObject.h"

namespace urchin
{

	/**
	* @param innerMargin Collision inner margin (must be equals to sphere radius)
	*/
	CollisionSphereShape::CollisionSphereShape(float innerMargin) :
			CollisionShape3D(innerMargin),
			sphereShape(std::make_shared<SphereShape<float>>(innerMargin))
	{

	}

	CollisionShape3D::ShapeType CollisionSphereShape::getShapeType() const
	{
		return CollisionShape3D::SPHERE_SHAPE;
	}

	std::shared_ptr<ConvexShape3D<float>> CollisionSphereShape::getSingleShape() const
	{
		return sphereShape;
	}

	float CollisionSphereShape::getRadius() const
	{
		return sphereShape->getRadius();
	}

	std::shared_ptr<CollisionShape3D> CollisionSphereShape::scale(float scale) const
	{
		return std::make_shared<CollisionSphereShape>(sphereShape->getRadius() * scale);
	}

	AABBox<float> CollisionSphereShape::toAABBox(const PhysicsTransform &physicsTransform) const
	{
		const Point3<float> &position = physicsTransform.getPosition();

		return AABBox<float>(position - sphereShape->getRadius(), position + sphereShape->getRadius());
	}

	std::shared_ptr<CollisionConvexObject3D> CollisionSphereShape::toConvexObject(const PhysicsTransform &physicsTransform) const
	{
		const Point3<float> &position = physicsTransform.getPosition();
		return std::make_shared<CollisionSphereObject>(getInnerMargin(), position);
	}

	Vector3<float> CollisionSphereShape::computeLocalInertia(float mass) const
	{
		float localInertia = (2.0f/5.0f) * mass * sphereShape->getRadius() * sphereShape->getRadius();
		return Vector3<float>(localInertia, localInertia, localInertia);
	}

	float CollisionSphereShape::getMaxDistanceToCenter() const
	{
		return sphereShape->getRadius();
	}

	float CollisionSphereShape::getMinDistanceToCenter() const
	{
		return sphereShape->getRadius();
	}

}
