#include "shape/CollisionCapsuleShape.h"
#include "object/CollisionCapsuleObject.h"

namespace urchin
{

	CollisionCapsuleShape::CollisionCapsuleShape(float radius, float cylinderHeight, CapsuleShape<float>::CapsuleOrientation capsuleOrientation) :
			CollisionShape3D(),
			capsuleShape(std::make_shared<CapsuleShape<float>>(radius, cylinderHeight, capsuleOrientation)),
			lastConvexObject(nullptr)
	{
		computeSafeMargin();
	}

	CollisionCapsuleShape::~CollisionCapsuleShape()
	{
		delete lastConvexObject;
	}

	void CollisionCapsuleShape::computeSafeMargin()
	{
		float maximumMarginPercentage = ConfigService::instance()->getFloatValue("collisionShape.maximumMarginPercentage");
		float maximumSafeMargin = getRadius() * maximumMarginPercentage;

		refreshInnerMargin(maximumSafeMargin);
	}

	CollisionShape3D::ShapeType CollisionCapsuleShape::getShapeType() const
	{
		return CollisionShape3D::CAPSULE_SHAPE;
	}

	std::shared_ptr<ConvexShape3D<float>> CollisionCapsuleShape::getSingleShape() const
	{
		return capsuleShape;
	}

	float CollisionCapsuleShape::getRadius() const
	{
		return capsuleShape->getRadius();
	}

	float CollisionCapsuleShape::getCylinderHeight() const
	{
		return capsuleShape->getCylinderHeight();
	}

	CapsuleShape<float>::CapsuleOrientation CollisionCapsuleShape::getCapsuleOrientation() const
	{
		return capsuleShape->getCapsuleOrientation();
	}

	std::shared_ptr<CollisionShape3D> CollisionCapsuleShape::scale(float scale) const
	{
		return std::make_shared<CollisionCapsuleShape>(capsuleShape->getRadius() * scale,
				capsuleShape->getCylinderHeight() * scale, capsuleShape->getCapsuleOrientation());
	}

	AABBox<float> CollisionCapsuleShape::toAABBox(const PhysicsTransform &physicsTransform) const
	{
		Vector3<float> boxHalfSizes(getRadius(), getRadius(), getRadius());
		boxHalfSizes[getCapsuleOrientation()] += getCylinderHeight() / 2.0f;
		const Matrix3<float> &orientation = physicsTransform.retrieveOrientationMatrix();
		Point3<float> extend(
			boxHalfSizes.X * std::abs(orientation(0)) + boxHalfSizes.Y * std::abs(orientation(3)) + boxHalfSizes.Z * std::abs(orientation(6)),
			boxHalfSizes.X * std::abs(orientation(1)) + boxHalfSizes.Y * std::abs(orientation(4)) + boxHalfSizes.Z * std::abs(orientation(7)),
			boxHalfSizes.X * std::abs(orientation(2)) + boxHalfSizes.Y * std::abs(orientation(5)) + boxHalfSizes.Z * std::abs(orientation(8))
		);

		const Point3<float> &position = physicsTransform.getPosition();

		return AABBox<float>(position - extend, position + extend);
	}

	CollisionConvexObject3D *CollisionCapsuleShape::toConvexObject(const PhysicsTransform &physicsTransform) const
	{
		const Point3<float> &position = physicsTransform.getPosition();
		const Quaternion<float> &orientation = physicsTransform.getOrientation();

		float reducedRadius = getRadius() - getInnerMargin();

        if(lastConvexObject==nullptr)
        {
            lastConvexObject = new CollisionCapsuleObject(getInnerMargin(), reducedRadius, getCylinderHeight(), getCapsuleOrientation(), position, orientation);
            return lastConvexObject;
        }
		return new (lastConvexObject) CollisionCapsuleObject(getInnerMargin(), reducedRadius, getCylinderHeight(), getCapsuleOrientation(), position, orientation);
	}

	Vector3<float> CollisionCapsuleShape::computeLocalInertia(float mass) const
	{ //rough local inertia computed based on box including capsule.
		Vector3<float> boxSizes(getRadius()*2.0f, getRadius()*2.0f, getRadius()*2.0f);
		boxSizes[getCapsuleOrientation()] += getCylinderHeight();

		float widthSquare = boxSizes.X * boxSizes.X;
		float heightSquare = boxSizes.Y * boxSizes.Y;
		float depthSquare = boxSizes.Z * boxSizes.Z;

		float localInertia1 = (1.0f/12.0f) * mass * (heightSquare + depthSquare);
		float localInertia2 = (1.0f/12.0f) * mass * (widthSquare + depthSquare);
		float localInertia3 = (1.0f/12.0f) * mass * (widthSquare + heightSquare);
		return Vector3<float>(localInertia1, localInertia2, localInertia3);
	}

	float CollisionCapsuleShape::getMaxDistanceToCenter() const
	{
		return capsuleShape->getCylinderHeight()/2.0f + capsuleShape->getRadius();
	}

	float CollisionCapsuleShape::getMinDistanceToCenter() const
	{
		return capsuleShape->getRadius();
	}

	CollisionShape3D *CollisionCapsuleShape::clone() const
	{
		return new CollisionCapsuleShape(capsuleShape->getRadius(), capsuleShape->getCylinderHeight(), capsuleShape->getCapsuleOrientation());
	}

}
