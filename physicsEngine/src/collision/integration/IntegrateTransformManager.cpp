#include "collision/integration/IntegrateTransformManager.h"
#include "shape/CollisionSphereShape.h"
#include "object/TemporalObject.h"

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
					handleContinuousCollision(body, currentTransform, newTransform);
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

	void IntegrateTransformManager::handleContinuousCollision(WorkRigidBody *body, const PhysicsTransform &from, const PhysicsTransform &to)
	{
		std::vector<AbstractWorkBody *> bodiesAABBoxHitBody = broadPhaseManager->bodyTest(body, from, to);
		if(bodiesAABBoxHitBody.size() > 0)
		{
			std::cout<<"Body needs CCD: "<<body->getId()<<std::endl;
			std::cout<<" - Broadphase: "<<std::endl;
			for(unsigned int i=0; i<bodiesAABBoxHitBody.size(); ++i)
			{
				std::cout<<"    - Body: "<<bodiesAABBoxHitBody[i]->getId()<<std::endl;
			}

			std::shared_ptr<CollisionSphereShape> bodySphereShape = body->getShape()->retrieveSphereShape();
			TemporalObject temporalObject(bodySphereShape.get(), from, to);
			ccd_set ccdResults = narrowPhaseManager->continuousCollissionTest(temporalObject, bodiesAABBoxHitBody);
			std::cout<<" - Narrowphase: "<<std::endl;
			for(const std::shared_ptr<ContinuousCollisionResult<float>> &ccdResult : ccdResults)
			{
				std::cout<<"    - Hit: "<<ccdResult->getTimeToHit()<<std::endl;
			}

			//TODO Update transform & reduce linear velocity
			//TODO Handle compound objects: not convex (train...) + ccdMotionThreshold define by body shape
			//TODO Attention to two objects moving in opposite direction: is it working ?
		}
	}

}
