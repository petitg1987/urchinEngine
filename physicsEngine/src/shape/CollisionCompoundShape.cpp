#include <stdexcept>
#include <limits>

#include "shape/CollisionSphereShape.h"
#include "shape/CollisionCompoundShape.h"

namespace urchin
{

	CollisionCompoundShape::CollisionCompoundShape(const std::vector<std::shared_ptr<const LocalizedCollisionShape>> &localizedShapes) :
			CollisionShape3D(),
			localizedShapes(localizedShapes)
	{
		if(localizedShapes.size()==0)
		{
			throw std::invalid_argument("Compound shape must be composed of at least one shape.");
		}

		initializeDistances();
	}

	CollisionCompoundShape::~CollisionCompoundShape()
	{

	}

	void CollisionCompoundShape::initializeDistances()
	{
		maxDistanceToCenter = toAABBox(PhysicsTransform()).getMaxHalfSize();

		minDistanceToCenter = std::numeric_limits<float>::max();
		for(const auto &localizedShape : localizedShapes)
		{
			if(localizedShape->shape->getMinDistanceToCenter() < minDistanceToCenter)
			{
				minDistanceToCenter = localizedShape->shape->getMinDistanceToCenter();
			}
		}
	}

	CollisionShape3D::ShapeType CollisionCompoundShape::getShapeType() const
	{
		return CollisionShape3D::COMPOUND_SHAPE;
	}

	const std::vector<std::shared_ptr<const LocalizedCollisionShape>> &CollisionCompoundShape::getLocalizedShapes() const
	{
		return localizedShapes;
	}

	std::shared_ptr<CollisionShape3D> CollisionCompoundShape::scale(float scale) const
	{
		std::vector<std::shared_ptr<const LocalizedCollisionShape>> scaledLocalizedShapes;
		for(unsigned int i=0; i<localizedShapes.size(); ++i)
		{
			std::shared_ptr<LocalizedCollisionShape> localizedShape = std::make_shared<LocalizedCollisionShape>();
			localizedShape->position = localizedShapes[i]->position;
			localizedShape->shape = localizedShapes[i]->shape->scale(scale);
			localizedShape->transform = PhysicsTransform(localizedShapes[i]->transform.getPosition() * scale, localizedShapes[i]->transform.getOrientation());

			scaledLocalizedShapes.push_back(localizedShape);
		}

		return std::make_shared<CollisionCompoundShape>(scaledLocalizedShapes);
	}

	AABBox<float> CollisionCompoundShape::toAABBox(const PhysicsTransform &physicsTransform) const
	{
		PhysicsTransform shapeWorldTransform = physicsTransform * localizedShapes[0]->transform;
		AABBox<float> globalCompoundBox = localizedShapes[0]->shape->toAABBox(shapeWorldTransform);

		for(unsigned int i=1; i<localizedShapes.size(); ++i)
		{
			shapeWorldTransform = physicsTransform * localizedShapes[i]->transform;
			AABBox<float> compoundBox = localizedShapes[i]->shape->toAABBox(shapeWorldTransform);

			globalCompoundBox = globalCompoundBox.merge(compoundBox);
		}

		return globalCompoundBox;
	}

	std::shared_ptr<CollisionConvexObject3D> CollisionCompoundShape::toConvexObject(const PhysicsTransform &physicsTransform) const
	{
		throw std::runtime_error("To convex object unsupported for compound shape");
	}

	Vector3<float> CollisionCompoundShape::computeLocalInertia(float mass) const
	{
		AABBox<float> aabbox = toAABBox(PhysicsTransform());
		float width = 2.0 * aabbox.getHalfSize(0);
		float height = 2.0 * aabbox.getHalfSize(1);
		float depth = 2.0 * aabbox.getHalfSize(2);

		float localInertia1 = (1.0/12.0) * mass * (height*height + depth*depth);
		float localInertia2 = (1.0/12.0) * mass * (width*width + depth*depth);
		float localInertia3 = (1.0/12.0) * mass * (width*width + height*height);
		return Vector3<float>(localInertia1, localInertia2, localInertia3);
	}

	float CollisionCompoundShape::getMaxDistanceToCenter() const
	{
		return maxDistanceToCenter;
	}

	float CollisionCompoundShape::getMinDistanceToCenter() const
	{
		return minDistanceToCenter;
	}

}
