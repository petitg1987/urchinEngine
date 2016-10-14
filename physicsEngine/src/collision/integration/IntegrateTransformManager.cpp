#include "collision/integration/IntegrateTransformManager.h"

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

				AABBox<float> bodyBox = body->getShape()->toAABBox(currentTransform); //TODO too slow ? propose sphere by default ?

				//TODO
				//1. Check illigible to CCD
				//2. Use broadphaseManager#enlargeRayTest() with object sphere
				//3. Create method for object in narrowManager and use it

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
