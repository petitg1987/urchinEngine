#include <cmath>

#include "shape/CollisionBoxShape.h"
#include "object/CollisionBoxObject.h"

namespace urchin
{

	CollisionBoxShape::CollisionBoxShape(const Vector3<float> &halfSizes) :
			CollisionShape3D(),
			boxShape(std::make_shared<BoxShape<float>>(halfSizes))
	{
		computeSafeMargin();

		lastTransform.setPosition(Point3<float>(NAN, NAN, NAN));
	}

	void CollisionBoxShape::computeSafeMargin()
	{
		float maximumMarginPercentage = ConfigService::instance()->getFloatValue("collisionShape.maximumMarginPercentage");
		float maximumSafeMargin = boxShape->getMinHalfSize() * maximumMarginPercentage;

		refreshInnerMargin(maximumSafeMargin);
	}

	CollisionShape3D::ShapeType CollisionBoxShape::getShapeType() const
	{
		return CollisionShape3D::BOX_SHAPE;
	}

	std::shared_ptr<ConvexShape3D<float>> CollisionBoxShape::getSingleShape() const
	{
		return boxShape;
	}

	float CollisionBoxShape::getHalfSize(unsigned int index) const
	{
		return boxShape->getHalfSize(index);
	}

	const Vector3<float> &CollisionBoxShape::getHalfSizes() const
	{
		return boxShape->getHalfSizes();
	}

	std::shared_ptr<CollisionShape3D> CollisionBoxShape::scale(float scale) const
	{
		return std::make_shared<CollisionBoxShape>(boxShape->getHalfSizes() * scale);
	}

	AABBox<float> CollisionBoxShape::toAABBox(const PhysicsTransform &physicsTransform) const
	{
		if(!lastTransform.equals(physicsTransform))
		{
			const Matrix3<float> &orientation = physicsTransform.retrieveOrientationMatrix();
			Point3<float> extend(
					boxShape->getHalfSize(0) * std::abs(orientation[0]) + boxShape->getHalfSize(1) * std::abs(orientation[3]) + boxShape->getHalfSize(2) * std::abs(orientation[6]),
					boxShape->getHalfSize(0) * std::abs(orientation[1]) + boxShape->getHalfSize(1) * std::abs(orientation[4]) + boxShape->getHalfSize(2) * std::abs(orientation[7]),
					boxShape->getHalfSize(0) * std::abs(orientation[2]) + boxShape->getHalfSize(1) * std::abs(orientation[5]) + boxShape->getHalfSize(2) * std::abs(orientation[8])
			);

			const Point3<float> &position = physicsTransform.getPosition();

			lastAABBox = AABBox<float>(position - extend, position + extend);
			lastTransform = physicsTransform;
		}

		return lastAABBox;
	}

	std::shared_ptr<CollisionConvexObject3D> CollisionBoxShape::toConvexObject(const PhysicsTransform &physicsTransform) const
	{
		const Point3<float> &position = physicsTransform.getPosition();
		const Quaternion<float> &orientation = physicsTransform.getOrientation();

		Vector3<float> halfSizeSubtractMargin = boxShape->getHalfSizes() - Vector3<float>(getInnerMargin(), getInnerMargin(), getInnerMargin());
		return std::make_shared<CollisionBoxObject>(getInnerMargin(), halfSizeSubtractMargin, position, orientation);
	}

	Vector3<float> CollisionBoxShape::computeLocalInertia(float mass) const
	{
		float width = 2.0f * boxShape->getHalfSize(0);
		float height = 2.0f * boxShape->getHalfSize(1);
		float depth = 2.0f * boxShape->getHalfSize(2);

		float localInertia1 = (1.0f/12.0f) * mass * (height*height + depth*depth);
		float localInertia2 = (1.0f/12.0f) * mass * (width*width + depth*depth);
		float localInertia3 = (1.0f/12.0f) * mass * (width*width + height*height);
		return Vector3<float>(localInertia1, localInertia2, localInertia3);
	}

	float CollisionBoxShape::getMaxDistanceToCenter() const
	{
		return boxShape->getMaxHalfSize();
	}

	float CollisionBoxShape::getMinDistanceToCenter() const
	{
		return boxShape->getMinHalfSize();
	}

}
