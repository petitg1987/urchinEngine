#include <vector>
#include <cassert>

#include "AIWorldGenerator.h"

namespace urchin
{

	std::shared_ptr<AIWorld> AIWorldGenerator::generate(const Map *map)
	{
		std::shared_ptr<AIWorld> aiWorld = std::make_shared<AIWorld>();

		const std::list<SceneObject *> &sceneObjects = map->getSceneObjects();
		for(auto sceneObject : sceneObjects)
		{
			RigidBody *rigidBody = sceneObject->getRigidBody();
			std::vector<LocalizedShape> localizedShapes = extractionLocalizedShapes(rigidBody);
			for(const auto &localizedShape : localizedShapes)
			{
				AIObject aiObject(sceneObject->getName(), localizedShape.shape, localizedShape.worldTransform);
				aiWorld->addObject(aiObject);
			}
		}

		return aiWorld;
	}

	std::vector<LocalizedShape> AIWorldGenerator::extractionLocalizedShapes(RigidBody *rigidBody)
	{
		std::vector<LocalizedShape> localizedShapes;
		if(rigidBody!=nullptr)
		{
			std::shared_ptr<const CollisionShape3D> scaledCollisionShape3D = rigidBody->getScaledShape();
			if(scaledCollisionShape3D->getShapeType()==CollisionShape3D::COMPOUND_SHAPE)
			{
				std::shared_ptr<const CollisionCompoundShape> collisionCompoundShape = std::dynamic_pointer_cast<const CollisionCompoundShape>(scaledCollisionShape3D);
				for(auto collisionLocalizedShape : collisionCompoundShape->getLocalizedShapes())
				{
					PhysicsTransform worldPhysicsTransform = PhysicsTransform(rigidBody->getTransform().getPosition(), rigidBody->getTransform().getOrientation()) * collisionLocalizedShape->transform;
					LocalizedShape localizedShape;
					localizedShape.shape = collisionLocalizedShape->shape->getSingleShape();
					localizedShape.worldTransform = worldPhysicsTransform.toTransform();

					localizedShapes.push_back(localizedShape);
				}
			}else
			{
				Transform<float> unscaledTransform = rigidBody->getTransform();
				unscaledTransform.setScale(1.0); //scale not needed because shape is already scaled.

				LocalizedShape localizedShape;
				localizedShape.shape = scaledCollisionShape3D->getSingleShape();
				localizedShape.worldTransform = unscaledTransform;

				localizedShapes.push_back(localizedShape);
			}
		}

		return localizedShapes;
	}

}
