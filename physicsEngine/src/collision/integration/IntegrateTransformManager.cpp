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
				PhysicsTransform newTransform = body->getPhysicsTransform().integrate(body->getLinearVelocity(), body->getAngularVelocity(), dt);

				float ccdMotionThreshold = body->getCcdMotionThreshold();
				float motion = currentTransform.getPosition().vector(newTransform.getPosition()).length();

				if(motion > ccdMotionThreshold)
				{
					handleContinuousCollision(body, currentTransform, newTransform, dt);
				}else
				{
					body->setPosition(newTransform.getPosition());
					body->setOrientation(newTransform.getOrientation());
				}
			}
		}
	}

	void IntegrateTransformManager::handleContinuousCollision(WorkRigidBody *body, const PhysicsTransform &from, const PhysicsTransform &to, float dt)
	{
		PhysicsTransform updatedTargetTransform = to;

		std::vector<AbstractWorkBody *> bodiesAABBoxHitBody = broadPhaseManager->bodyTest(body, from, to);
		if(bodiesAABBoxHitBody.size() > 0)
		{
			std::shared_ptr<CollisionSphereShape> bodySphereShape = body->getShape()->retrieveSphereShape();
			TemporalObject temporalObject(bodySphereShape.get(), from, to);
			ccd_set ccdResults = narrowPhaseManager->continuousCollissionTest(temporalObject, bodiesAABBoxHitBody);

			if(ccdResults.size() > 0)
			{
				//determine new body transform to avoid collision
				float timeToFirstHit = ccdResults.begin()->get()->getTimeToHit();
				updatedTargetTransform = from.integrate(body->getLinearVelocity(), body->getAngularVelocity(), timeToFirstHit*dt);

				//clamp velocity to max speed
				float maxSpeedAllowed = body->getCcdMotionThreshold() / dt;
				float currentSpeed = body->getLinearVelocity().length();
				if(currentSpeed > maxSpeedAllowed)
				{
					body->setLinearVelocity((body->getLinearVelocity() / currentSpeed) * maxSpeedAllowed);
				}

				//TODO Handle compound objects:
				// - ccdMotionThreshold should be defined by body or shape ?
				// - Crash in narrow phase if TemporalObject is created for compound & crash in ray test if ray hit compound shape
			}
		}

		body->setPosition(updatedTargetTransform.getPosition());
		body->setOrientation(updatedTargetTransform.getOrientation());
	}
}
