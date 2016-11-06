#include "shape/CollisionBoxShape.h"
#include "shape/CollisionSphereShape.h"
#include "object/CollisionBoxObject.h"

namespace urchin
{

	CollisionBoxShape::CollisionBoxShape(const Vector3<float> &halfSizes) :
			CollisionShape3D(),
			boxShape(BoxShape<float>(halfSizes))
	{
		computeSafeMargin();
	}

	CollisionBoxShape::CollisionBoxShape(float innerMargin, const Vector3<float> &halfSizes) :
			CollisionShape3D(innerMargin),
			boxShape(BoxShape<float>(halfSizes))
	{
		computeSafeMargin();
	}

	CollisionBoxShape::~CollisionBoxShape()
	{

	}

	void CollisionBoxShape::computeSafeMargin()
	{
		const Vector3<float> &halfSizes = getHalfSizes();
		float minAxis = (halfSizes.X < halfSizes.Y) ? ((halfSizes.X < halfSizes.Z) ? halfSizes.X : halfSizes.Z) : ((halfSizes.Y < halfSizes.Z) ? halfSizes.Y : halfSizes.Z);
		float maximumMarginPercentage = ConfigService::instance()->getFloatValue("collisionShape.maximumMarginPercentage");
		float maximumSafeMargin = minAxis * maximumMarginPercentage;

		refreshInnerMargin(maximumSafeMargin);
	}

	CollisionShape3D::ShapeType CollisionBoxShape::getShapeType() const
	{
		return CollisionShape3D::BOX_SHAPE;
	}

	float CollisionBoxShape::getHalfSize(unsigned int index) const
	{
		return boxShape.getHalfSize(index);
	}

	const Vector3<float> &CollisionBoxShape::getHalfSizes() const
	{
		return boxShape.getHalfSizes();
	}

	std::shared_ptr<CollisionShape3D> CollisionBoxShape::scale(float scale) const
	{
		return std::make_shared<CollisionBoxShape>(boxShape.getHalfSizes() * scale);
	}

	std::shared_ptr<CollisionSphereShape> CollisionBoxShape::retrieveSphereShape() const
	{
		return std::make_shared<CollisionSphereShape>(boxShape.getMinHalfSize());
	}

	AABBox<float> CollisionBoxShape::toAABBox(const PhysicsTransform &physicsTransform) const
	{
		const Point3<float> &position = physicsTransform.getPosition();
		const Matrix3<float> &orientation = physicsTransform.retrieveOrientationMatrix();

		Point3<float> extend(
			boxShape.getHalfSize(0) * std::abs(orientation[0]) + boxShape.getHalfSize(1) * std::abs(orientation[3]) + boxShape.getHalfSize(2) * std::abs(orientation[6]),
			boxShape.getHalfSize(0) * std::abs(orientation[1]) + boxShape.getHalfSize(1) * std::abs(orientation[4]) + boxShape.getHalfSize(2) * std::abs(orientation[7]),
			boxShape.getHalfSize(0) * std::abs(orientation[2]) + boxShape.getHalfSize(1) * std::abs(orientation[5]) + boxShape.getHalfSize(2) * std::abs(orientation[8])
		);

		return AABBox<float>(position - extend, position + extend);
	}

	std::shared_ptr<CollisionConvexObject3D> CollisionBoxShape::toConvexObject(const PhysicsTransform &physicsTransform) const
	{
		const Point3<float> &position = physicsTransform.getPosition();
		const Quaternion<float> &orientation = physicsTransform.getOrientation();

		Vector3<float> halfSizeSubtractMargin = boxShape.getHalfSizes() - Vector3<float>(getInnerMargin(), getInnerMargin(), getInnerMargin());
		return std::make_shared<CollisionBoxObject>(getInnerMargin(), halfSizeSubtractMargin, position, orientation);
	}

	Vector3<float> CollisionBoxShape::computeLocalInertia(float mass) const
	{
		float width = 2.0 * boxShape.getHalfSize(0);
		float height = 2.0 * boxShape.getHalfSize(1);
		float depth = 2.0 * boxShape.getHalfSize(2);

		float localInertia1 = (1.0/12.0) * mass * (height*height + depth*depth);
		float localInertia2 = (1.0/12.0) * mass * (width*width + depth*depth);
		float localInertia3 = (1.0/12.0) * mass * (width*width + height*height);
		return Vector3<float>(localInertia1, localInertia2, localInertia3);
	}

}
