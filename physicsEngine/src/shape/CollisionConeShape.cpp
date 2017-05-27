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
		boxHalfSizes[getConeOrientation()/2] = getHeight() / 2.0f;
		const Matrix3<float> &orientation = physicsTransform.retrieveOrientationMatrix();
		Point3<float> extend(
			boxHalfSizes.X * std::abs(orientation[0]) + boxHalfSizes.Y * std::abs(orientation[3]) + boxHalfSizes.Z * std::abs(orientation[6]),
			boxHalfSizes.X * std::abs(orientation[1]) + boxHalfSizes.Y * std::abs(orientation[4]) + boxHalfSizes.Z * std::abs(orientation[7]),
			boxHalfSizes.X * std::abs(orientation[2]) + boxHalfSizes.Y * std::abs(orientation[5]) + boxHalfSizes.Z * std::abs(orientation[8])
		);

		const Point3<float> &centerOfMass = physicsTransform.getPosition();
		Point3<float> localCentralAxis(0.0, 0.0, 0.0);
		localCentralAxis[getConeOrientation()/2] = (getConeOrientation()%2==0) ? 1.0f : -1.0f;
		Vector3<float> centralAxis = physicsTransform.getOrientation().rotatePoint(localCentralAxis).toVector();
		Point3<float> centerPosition = centerOfMass.translate(centralAxis * (1.0f/4.0f)*getHeight());

		return AABBox<float>(centerPosition - extend, centerPosition + extend);
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
		float interiaValue = (3.0f/20.0f) * mass * (getRadius()*getRadius() + 4.0f*getHeight()*getHeight());

		Vector3<float> inertia(interiaValue, interiaValue, interiaValue);
		inertia[getConeOrientation()/2] = (3.0f/10.0f) * mass * getRadius() * getRadius();

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
