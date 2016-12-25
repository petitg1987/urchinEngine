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
				//TODO common:
				//1) Allow to convert ConvexShape3D into ConvexObject3D with a Transform
				//2) Use this above method in physics engine ?

				//std::shared_ptr<const CollisionShape3D> collisionShape3D = rigidBody->getScaledShape(); //TODO transform/get ConvexShape3D ?
				//const Transform<float> &transform = rigidBody->getTransform(); //TODO: create AITransform without scale ?
			}
		}

		return aiWorld;
	}

}
