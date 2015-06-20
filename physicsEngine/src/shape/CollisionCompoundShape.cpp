#include <stdexcept>

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
	}

	CollisionCompoundShape::~CollisionCompoundShape()
	{

	}

	CollisionShape3D::ShapeType CollisionCompoundShape::getShapeType() const
	{
		return CollisionShape3D::COMPOUND_SHAPE;
	}

	std::shared_ptr<CollisionShape3D> CollisionCompoundShape::scale(float scale) const
	{
		std::vector<std::shared_ptr<const LocalizedCollisionShape>> scaledLocalizedShapes;
		for(unsigned int i=0; i<localizedShapes.size(); ++i)
		{
			std::shared_ptr<LocalizedCollisionShape> localizedShape = std::make_shared<LocalizedCollisionShape>();
			localizedShape->shape = localizedShapes[i]->shape->scale(scale);
			localizedShape->translation = localizedShapes[i]->translation * scale;

			scaledLocalizedShapes.push_back(localizedShape);
		}

		return std::make_shared<CollisionCompoundShape>(scaledLocalizedShapes);
	}

	AABBox<float> CollisionCompoundShape::toAABBox(const PhysicsTransform &physicsTransform) const
	{
		Point3<float> rotatedTranslation = physicsTransform.getOrientation().rotatePoint(Point3<float>(localizedShapes[0]->translation));
		Point3<float> finalPosition = physicsTransform.getPosition().translate(rotatedTranslation.toVector());
		PhysicsTransform shapeWorldTransform(finalPosition, physicsTransform.getOrientation());
		AABBox<float> globalCompoundBox = localizedShapes[0]->shape->toAABBox(shapeWorldTransform);

		for(unsigned int i=1; i<localizedShapes.size(); ++i)
		{
			rotatedTranslation = physicsTransform.getOrientation().rotatePoint(Point3<float>(localizedShapes[i]->translation));
			finalPosition = physicsTransform.getPosition().translate(rotatedTranslation.toVector());
			shapeWorldTransform.setPosition(finalPosition);

			AABBox<float> compoundBox = localizedShapes[i]->shape->toAABBox(shapeWorldTransform);

			globalCompoundBox = globalCompoundBox.merge(compoundBox);
		}

		return globalCompoundBox;
	}

	std::shared_ptr<CollisionConvexObject3D> CollisionCompoundShape::toConvexObject(const PhysicsTransform &physicsTransform) const
	{
		throw std::runtime_error("Not implemented");
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

	const std::vector<std::shared_ptr<const LocalizedCollisionShape>> &CollisionCompoundShape::getLocalizedShapes() const
	{
		return localizedShapes;
	}

}
