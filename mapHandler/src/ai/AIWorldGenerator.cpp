#include "UrchinPhysicsEngine.h"

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
			if(rigidBody!=nullptr)
			{
				//std::shared_ptr<const CollisionShape3D> collisionShape3D = rigidBody->getScaledShape(); //TODO add getConvexShape3D() ?
				//const Transform<float> &transform = rigidBody->getTransform(); //TODO combine with transform of compound shape
			}
		}

		return aiWorld;
	}

}
