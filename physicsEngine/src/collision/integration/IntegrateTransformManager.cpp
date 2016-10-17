#include "collision/integration/IntegrateTransformManager.h"
#include <shape/CollisionSphereShape.h>

namespace urchin
{

	IntegrateTransformManager::IntegrateTransformManager(const BodyManager *bodyManager,
			const BroadPhaseManager *broadPhaseManager, const NarrowPhaseManager *narrowPhaseManager) :
			bodyManager(bodyManager),
			broadPhaseManager(broadPhaseManager),
			narrowPhaseManager(narrowPhaseManager)
	{

	}

	IntegrateTransformManager::~IntegrateTransformManager()
	{

	}

	/**
	 * @param dt Delta of time between two simulation steps
	 */
	void IntegrateTransformManager::integrateTransform(float dt)
	{
		for(unsigned int i=0; i<bodyManager->getWorkBodies().size(); ++i)
		{
			WorkRigidBody *body = WorkRigidBody::upCast(bodyManager->getWorkBodies()[i]);
			if(body!=nullptr && body->isActive())
			{
				const PhysicsTransform &currentTransform = body->getPhysicsTransform();
				PhysicsTransform newTransform = computeNewIntegrateTransform(dt, body);

				float ccdMotionThreshold = body->getCcdMotionThreshold();
				float motion = currentTransform.getPosition().vector(newTransform.getPosition()).length();

				if(motion > ccdMotionThreshold)
				{
					std::cout<<"Body needs CCD: "<<body->getId()<<std::endl;

					std::shared_ptr<CollisionSphereShape> bodySphereShape = body->getShape()->retrieveSphereShape();

					Ray<float> ray(currentTransform.getPosition(), newTransform.getPosition());
					Vector3<float> enlargeBoxHalfSize(bodySphereShape->getRadius(), bodySphereShape->getRadius(), bodySphereShape->getRadius());
					std::vector<AbstractWorkBody *> bodiesAABBoxHitEnlargedRay = broadPhaseManager->enlargedRayTest(ray, enlargeBoxHalfSize);

					for(unsigned int i=0; i<bodiesAABBoxHitEnlargedRay.size(); ++i)
					{
						AbstractWorkBody *currentBody = bodiesAABBoxHitEnlargedRay[i];
						if(currentBody==body)
						{
							continue;
						}

						std::cout<<" - Body to check:"<<currentBody->getId()<<std::endl;
						//1. Create method for object in narrowManager and use it
						//2. Update integration (/!\: reduce velocity ?!)
						//Attention to compound objects: not convex (train...) + ccdMotionThreshold define by body shape
						//Attention to two objects moving in opposite direction: is it working ?
					}
				}

				body->setPosition(newTransform.getPosition());
				body->setOrientation(newTransform.getOrientation());
			}
		}
	}

	PhysicsTransform IntegrateTransformManager::computeNewIntegrateTransform(float dt, const WorkRigidBody *body) const
	{
		PhysicsTransform newTransform;

		//update position
		newTransform.setPosition(body->getPosition().translate(body->getLinearVelocity() * dt));

		//update orientation
		float length = body->getAngularVelocity().length();
		if(length > 0.0)
		{
			const Vector3<float> normalizedAxis = body->getAngularVelocity() / length;
			const float angle = length * dt;

			Quaternion<float> newOrientation = Quaternion<float>(normalizedAxis, angle) * body->getOrientation();
			newOrientation = newOrientation.normalize();
			newTransform.setOrientation(newOrientation);
		}

		return newTransform;
	}

}
