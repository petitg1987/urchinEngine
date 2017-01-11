#include "shape/CollisionConeShape.h"
#include "object/CollisionConeObject.h"

namespace urchin
{

	CollisionConeShape::CollisionConeShape(float radius, float height, ConeShape<float>::ConeOrientation coneOrientation) :
			CollisionShape3D(),
			coneShape(std::make_shared<ConeShape<float>>(radius, height, coneOrientation))
	{
		computeSafeMargin();
	}

	CollisionConeShape::CollisionConeShape(float innerMargin, float radius, float height, ConeShape<float>::ConeOrientation coneOrientation) :
			CollisionShape3D(innerMargin),
			coneShape(std::make_shared<ConeShape<float>>(radius, height, coneOrientation))
	{
		computeSafeMargin();
	}

	CollisionConeShape::~CollisionConeShape()
	{

	}

	void CollisionConeShape::computeSafeMargin()
	{
		float minAxis = std::min(getRadius(), getHeight() / 2.0f);
		float maximumMarginPercentage = ConfigService::instance()->getFloatValue("collisionShape.maximumMarginPercentage");
		float maximumSafeMargin = minAxis * maximumMarginPercentage;

		refreshInnerMargin(maximumSafeMargin);
	}

	CollisionShape3D::ShapeType CollisionConeShape::getShapeType() const
	{
		return CollisionShape3D::CONE_SHAPE;
	}

	std::shared_ptr<ConvexShape3D<float>> CollisionConeShape::getSingleShape() const
	{
		return coneShape;
	}

	float CollisionConeShape::getRadius() const
	{
		return coneShape->getRadius();
	}

	float CollisionConeShape::getHeight() const
	{
		return coneShape->getHeight();
	}

	ConeShape<float>::ConeOrientation CollisionConeShape::getConeOrientation() const
	{
		return coneShape->getConeOrientation();
	}

	std::shared_ptr<CollisionShape3D> CollisionConeShape::scale(float scale) const
	{
		return std::make_shared<CollisionConeShape>(coneShape->getRadius() * scale,
				coneShape->getHeight() * scale, coneShape->getConeOrientation());
	}

	AABBox<float> CollisionConeShape::toAABBox(const PhysicsTransform &physicsTransform) const
	{
		Vector3<float> boxHalfSizes(getRadius(), getRadius(), getRadius());
		boxHalfSizes[getConeOrientation()/2] = getHeight() / 2.0;

		const Point3<float> &position = physicsTransform.getPosition();
		const Matrix3<float> &orientation = physicsTransform.retrieveOrientationMatrix();

		Point3<float> extend(
			boxHalfSizes.X * std::abs(orientation[0]) + boxHalfSizes.Y * std::abs(orientation[3]) + boxHalfSizes.Z * std::abs(orientation[6]),
			boxHalfSizes.X * std::abs(orientation[1]) + boxHalfSizes.Y * std::abs(orientation[4]) + boxHalfSizes.Z * std::abs(orientation[7]),
			boxHalfSizes.X * std::abs(orientation[2]) + boxHalfSizes.Y * std::abs(orientation[5]) + boxHalfSizes.Z * std::abs(orientation[8])
		);

		return AABBox<float>(position - extend, position + extend);
	}

	std::shared_ptr<CollisionConvexObject3D> CollisionConeShape::toConvexObject(const PhysicsTransform &physicsTransform) const
	{
		const Point3<float> &position = physicsTransform.getPosition();
		const Quaternion<float> &orientation = physicsTransform.getOrientation();

		float reducedRadius = getRadius() - getInnerMargin();
		float reducedHeight = getHeight() - (2.0f * getInnerMargin());
		return std::make_shared<CollisionConeObject>(getInnerMargin(), reducedRadius, reducedHeight, getConeOrientation(), position, orientation);
	}

	Vector3<float> CollisionConeShape::computeLocalInertia(float mass) const
	{
		float interiaValue = ((3.0 * mass)/20.0) * (getRadius()*getRadius() + 4.0*getHeight()*getHeight());

		Vector3<float> inertia(interiaValue, interiaValue, interiaValue);
		inertia[getConeOrientation()/2] = (3.0 * mass * getRadius() * getRadius()) / 10.0;

		return inertia;
	}

	float CollisionConeShape::getMaxDistanceToCenter() const
	{
		return std::max(coneShape->getHeight()/2.0f, coneShape->getRadius());
	}

	float CollisionConeShape::getMinDistanceToCenter() const
	{
		return std::min(coneShape->getHeight()/2.0f, coneShape->getRadius());
	}

}
