#include "shape/CollisionCylinderShape.h"
#include "object/CollisionCylinderObject.h"

namespace urchin
{

	CollisionCylinderShape::CollisionCylinderShape(float radius, float height, CylinderShape<float>::CylinderOrientation cylinderOrientation) :
			CollisionShape3D(),
			cylinderShape(std::make_shared<CylinderShape<float>>(radius, height, cylinderOrientation))
	{
		computeSafeMargin();
	}

	CollisionCylinderShape::CollisionCylinderShape(float innerMargin, float radius, float height, CylinderShape<float>::CylinderOrientation cylinderOrientation) :
			CollisionShape3D(innerMargin),
			cylinderShape(std::make_shared<CylinderShape<float>>(radius, height, cylinderOrientation))
	{
		computeSafeMargin();
	}

	CollisionCylinderShape::~CollisionCylinderShape()
	{

	}

	void CollisionCylinderShape::computeSafeMargin()
	{
		float minAxis = std::min(getRadius(), getHeight() / 2.0f);
		float maximumMarginPercentage = ConfigService::instance()->getFloatValue("collisionShape.maximumMarginPercentage");
		float maximumSafeMargin = minAxis * maximumMarginPercentage;

		refreshInnerMargin(maximumSafeMargin);
	}

	CollisionShape3D::ShapeType CollisionCylinderShape::getShapeType() const
	{
		return CollisionShape3D::CYLINDER_SHAPE;
	}

	std::shared_ptr<ConvexShape3D<float>> CollisionCylinderShape::getSingleShape() const
	{
		return cylinderShape;
	}

	float CollisionCylinderShape::getRadius() const
	{
		return cylinderShape->getRadius();
	}

	float CollisionCylinderShape::getHeight() const
	{
		return cylinderShape->getHeight();
	}

	CylinderShape<float>::CylinderOrientation CollisionCylinderShape::getCylinderOrientation() const
	{
		return cylinderShape->getCylinderOrientation();
	}

	std::shared_ptr<CollisionShape3D> CollisionCylinderShape::scale(float scale) const
	{
		return std::make_shared<CollisionCylinderShape>(cylinderShape->getRadius() * scale,
				cylinderShape->getHeight() * scale, cylinderShape->getCylinderOrientation());
	}

	AABBox<float> CollisionCylinderShape::toAABBox(const PhysicsTransform &physicsTransform) const
	{
		Vector3<float> boxHalfSizes(getRadius(), getRadius(), getRadius());
		boxHalfSizes[getCylinderOrientation()] = getHeight() / 2.0f;

		const Point3<float> &position = physicsTransform.getPosition();
		const Matrix3<float> &orientation = physicsTransform.retrieveOrientationMatrix();

		Point3<float> extend(
			boxHalfSizes.X * std::abs(orientation[0]) + boxHalfSizes.Y * std::abs(orientation[3]) + boxHalfSizes.Z * std::abs(orientation[6]),
			boxHalfSizes.X * std::abs(orientation[1]) + boxHalfSizes.Y * std::abs(orientation[4]) + boxHalfSizes.Z * std::abs(orientation[7]),
			boxHalfSizes.X * std::abs(orientation[2]) + boxHalfSizes.Y * std::abs(orientation[5]) + boxHalfSizes.Z * std::abs(orientation[8])
		);

		return AABBox<float>(position - extend, position + extend);
	}

	std::shared_ptr<CollisionConvexObject3D> CollisionCylinderShape::toConvexObject(const PhysicsTransform &physicsTransform) const
	{
		const Point3<float> &position = physicsTransform.getPosition();
		const Quaternion<float> &orientation = physicsTransform.getOrientation();

		float reducedRadius = getRadius() - getInnerMargin();
		float reducedHeight = getHeight() - (2.0f * getInnerMargin());
		return std::make_shared<CollisionCylinderObject>(getInnerMargin(), reducedRadius, reducedHeight, getCylinderOrientation(), position, orientation);
	}

	Vector3<float> CollisionCylinderShape::computeLocalInertia(float mass) const
	{
		float interiaValue = (1.0f/12.0f) * mass * (3.0f*getRadius()*getRadius() + getHeight()*getHeight());

		Vector3<float> inertia(interiaValue, interiaValue, interiaValue);
		inertia[getCylinderOrientation()] = 0.5f * mass * getRadius() * getRadius();

		return inertia;
	}

	float CollisionCylinderShape::getMaxDistanceToCenter() const
	{
		return std::max(cylinderShape->getHeight()/2.0f, cylinderShape->getRadius());
	}

	float CollisionCylinderShape::getMinDistanceToCenter() const
	{
		return std::min(cylinderShape->getHeight()/2.0f, cylinderShape->getRadius());
	}

}
