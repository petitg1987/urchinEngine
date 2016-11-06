#include "DefaultBodyShapeCreator.h"

namespace urchin
{

	DefaultBodyShapeCreator::DefaultBodyShapeCreator(const SceneObject *sceneObject) :
		sceneObject(sceneObject)
	{

	}

	std::shared_ptr<const CollisionShape3D> DefaultBodyShapeCreator::createDefaultBodyShape(CollisionShape3D::ShapeType shapeType, bool scaled) const
	{
		CollisionShape3D *shape;
		const AABBox<float> &modelAABBox = sceneObject->getModel()->getLocalAABBox();

		if(shapeType==CollisionShape3D::ShapeType::BOX_SHAPE)
		{
			shape = new CollisionBoxShape(modelAABBox.getHalfSizes());
		}else if(shapeType==CollisionShape3D::ShapeType::SPHERE_SHAPE)
		{
			shape = new CollisionSphereShape(modelAABBox.getMaxHalfSize());
		}else if(shapeType==CollisionShape3D::ShapeType::CAPSULE_SHAPE)
		{
			float radius = std::max(modelAABBox.getHalfSizes()[1]*2.0f,  modelAABBox.getHalfSizes()[2]*2.0f);
			float cylinderHeight = modelAABBox.getHalfSizes()[0]*2.0f;

			shape = new CollisionCapsuleShape(radius, cylinderHeight, CapsuleShape<float>::CAPSULE_X);
		}else if(shapeType==CollisionShape3D::ShapeType::CYLINDER_SHAPE)
		{
			float radius = std::max(modelAABBox.getHalfSizes()[1]*2.0f,  modelAABBox.getHalfSizes()[2]*2.0f);
			float height = modelAABBox.getHalfSizes()[0]*2.0f;

			shape = new CollisionCylinderShape(radius, height, CylinderShape<float>::CYLINDER_X);
		}else if(shapeType==CollisionShape3D::ShapeType::CONVEX_HULL_SHAPE)
		{
			shape = new CollisionConvexHullShape(modelAABBox.getEightPoints());
		} else if(shapeType==CollisionShape3D::ShapeType::COMPOUND_SHAPE)
		{
			std::vector<std::shared_ptr<const LocalizedCollisionShape>> localizedCollisionShapes;

			std::shared_ptr<LocalizedCollisionShape> boxLocalizedShape = std::make_shared<LocalizedCollisionShape>();
			boxLocalizedShape->position = 0;
			boxLocalizedShape->shape = std::make_shared<const CollisionBoxShape>(modelAABBox.getHalfSizes());
			boxLocalizedShape->transform = PhysicsTransform();
			localizedCollisionShapes.push_back(boxLocalizedShape);

			shape = new CollisionCompoundShape(localizedCollisionShapes);
		}else
		{
			throw std::invalid_argument("Unknown shape type to create default body shape: " + shapeType);
		}

		if(scaled)
		{
			float scale = sceneObject->getModel()->getTransform().getScale();
			std::shared_ptr<const CollisionShape3D> scaledShape = shape->scale(scale);
			delete shape;

			return scaledShape;
		}
		return std::shared_ptr<const CollisionShape3D>(shape);
	}
}
