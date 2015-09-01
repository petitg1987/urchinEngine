#include "shape/CollisionCapsuleShape.h"
#include "object/CollisionCapsuleObject.h"

namespace urchin
{

	CollisionCapsuleShape::CollisionCapsuleShape(float radius, float cylinderHeight, CapsuleShape<float>::CapsuleOrientation capsuleOrientation) :
			CollisionShape3D(),
			capsuleShape(CapsuleShape<float>(radius, cylinderHeight, capsuleOrientation))
	{
		computeSafeMargin();
	}

	CollisionCapsuleShape::CollisionCapsuleShape(float innerMargin, float radius, float cylinderHeight, CapsuleShape<float>::CapsuleOrientation capsuleOrientation) :
			CollisionShape3D(innerMargin),
			capsuleShape(CapsuleShape<float>(radius, cylinderHeight, capsuleOrientation))
	{
		computeSafeMargin();
	}

	CollisionCapsuleShape::~CollisionCapsuleShape()
	{

	}

	void CollisionCapsuleShape::computeSafeMargin()
	{
		float maximumMarginPercentage = ConfigService::instance()->getFloatValue("collisionShape.maximumMarginPercentage");
		float minimumSafeMargin = getRadius() * maximumMarginPercentage;

		if(minimumSafeMargin < getInnerMargin())
		{
			setInnerMargin(minimumSafeMargin);
		}
	}

	CollisionShape3D::ShapeType CollisionCapsuleShape::getShapeType() const
	{
		return CollisionShape3D::CAPSULE_SHAPE;
	}

	float CollisionCapsuleShape::getRadius() const
	{
		return capsuleShape.getRadius();
	}

	float CollisionCapsuleShape::getCylinderHeight() const
	{
		return capsuleShape.getCylinderHeight();
	}

	CapsuleShape<float>::CapsuleOrientation CollisionCapsuleShape::getCapsuleOrientation() const
	{
		return capsuleShape.getCapsuleOrientation();
	}

	std::shared_ptr<CollisionShape3D> CollisionCapsuleShape::scale(float scale) const
	{
		return std::make_shared<CollisionCapsuleShape>(capsuleShape.getRadius() * scale,
				capsuleShape.getCylinderHeight() * scale, capsuleShape.getCapsuleOrientation());
	}

	AABBox<float> CollisionCapsuleShape::toAABBox(const PhysicsTransform &physicsTransform) const
	{
		Vector3<float> boxHalfSizes(getRadius(), getRadius(), getRadius());
		boxHalfSizes[getCapsuleOrientation()] += getCylinderHeight() / 2.0;

		const Point3<float> &position = physicsTransform.getPosition();
		const Matrix3<float> &orientation = physicsTransform.getOrientationMatrix();

		Point3<float> extend(
			boxHalfSizes.X * std::abs(orientation[0]) + boxHalfSizes.Y * std::abs(orientation[3]) + boxHalfSizes.Z * std::abs(orientation[6]),
			boxHalfSizes.X * std::abs(orientation[1]) + boxHalfSizes.Y * std::abs(orientation[4]) + boxHalfSizes.Z * std::abs(orientation[7]),
			boxHalfSizes.X * std::abs(orientation[2]) + boxHalfSizes.Y * std::abs(orientation[5]) + boxHalfSizes.Z * std::abs(orientation[8])
		);

		return AABBox<float>(position - extend, position + extend);
	}

	std::shared_ptr<CollisionConvexObject3D> CollisionCapsuleShape::toConvexObject(const PhysicsTransform &physicsTransform) const
	{
		const Point3<float> &position = physicsTransform.getPosition();
		const Quaternion<float> &orientation = physicsTransform.getOrientation();

		float reducedRadius = getRadius() - getInnerMargin();
		return std::make_shared<CollisionCapsuleObject>(getInnerMargin(), reducedRadius, getCylinderHeight(), getCapsuleOrientation(), position, orientation);
	}

	Vector3<float> CollisionCapsuleShape::computeLocalInertia(float mass) const
	{ //approximative local inertia computed based on box including capsule.
		Vector3<float> boxSizes(getRadius()*2.0, getRadius()*2.0, getRadius()*2.0);
		boxSizes[getCapsuleOrientation()] += getCylinderHeight();

		float widthSquare = boxSizes.X * boxSizes.X;
		float heightSquare = boxSizes.Y * boxSizes.Y;
		float depthSquare = boxSizes.Z * boxSizes.Z;

		float localInertia1 = (1.0/12.0) * mass * (heightSquare + depthSquare);
		float localInertia2 = (1.0/12.0) * mass * (widthSquare + depthSquare);
		float localInertia3 = (1.0/12.0) * mass * (widthSquare + heightSquare);
		return Vector3<float>(localInertia1, localInertia2, localInertia3);
	}

}
