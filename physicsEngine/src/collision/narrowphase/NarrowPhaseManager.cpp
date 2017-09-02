#include "collision/narrowphase/NarrowPhaseManager.h"
#include "shape/CollisionShape3D.h"
#include "shape/CollisionSphereShape.h"
#include "shape/CollisionCompoundShape.h"
#include "body/work/WorkRigidBody.h"
#include "object/TemporalObject.h"

namespace urchin
{

	NarrowPhaseManager::NarrowPhaseManager(const BodyManager *bodyManager, const BroadPhaseManager *broadPhaseManager) :
			bodyManager(bodyManager),
			broadPhaseManager(broadPhaseManager),
			collisionAlgorithmSelector(new CollisionAlgorithmSelector())
	{

	}

	NarrowPhaseManager::~NarrowPhaseManager()
	{
		delete collisionAlgorithmSelector;
	}

	/**
	 * @param dt Delta of time (sec.) between two simulation steps
	 * @param overlappingPairs Pairs of bodies potentially colliding
	 * @param manifoldResults [OUT] Collision constraints
	 */
	void NarrowPhaseManager::process(float dt, const std::vector<OverlappingPair *> &overlappingPairs, std::vector<ManifoldResult> &manifoldResults)
	{
		processOverlappingPairs(overlappingPairs, manifoldResults);
		processPredictiveContacts(dt, manifoldResults);
	}

	/**
	 * @param ghostBody Ghost body to process
	 * @param manifoldResults [OUT] Collision constraints
	 */
	void NarrowPhaseManager::processGhostBody(WorkGhostBody *ghostBody, std::vector<ManifoldResult> &manifoldResults)
	{
		const std::vector<OverlappingPair *> &overlappingPairs = ghostBody->getPairContainer()->getOverlappingPairs();
		processOverlappingPairs(overlappingPairs, manifoldResults);
	}

	void NarrowPhaseManager::processOverlappingPairs(const std::vector<OverlappingPair *> &overlappingPairs, std::vector<ManifoldResult> &manifoldResults)
	{
		for(const auto &overlappingPair : overlappingPairs)
		{
			AbstractWorkBody *body1 = overlappingPair->getBody1();
			AbstractWorkBody *body2 = overlappingPair->getBody2();

			if(body1->isActive() || body2->isActive())
			{
				std::shared_ptr<CollisionAlgorithm> collisionAlgorithm = retrieveCollisionAlgorithm(overlappingPair);
				const CollisionAlgorithm *const constCollisionAlgorithm = collisionAlgorithm.get();

				CollisionObjectWrapper collisionObject1(*body1->getShape(), body1->getPhysicsTransform());
				CollisionObjectWrapper collisionObject2(*body2->getShape(), body2->getPhysicsTransform());
				collisionAlgorithm->processCollisionAlgorithm(collisionObject1, collisionObject2, true);

				if(constCollisionAlgorithm->getConstManifoldResult().getNumContactPoints()!=0)
				{
					manifoldResults.push_back(constCollisionAlgorithm->getConstManifoldResult());
				}
			}
		}
	}

	std::shared_ptr<CollisionAlgorithm> NarrowPhaseManager::retrieveCollisionAlgorithm(OverlappingPair *overlappingPair)
	{
		std::shared_ptr<CollisionAlgorithm> collisionAlgorithm = overlappingPair->getCollisionAlgorithm();
		if(collisionAlgorithm==nullptr)
		{
			AbstractWorkBody *body1 = overlappingPair->getBody1();
			AbstractWorkBody *body2 = overlappingPair->getBody2();

			collisionAlgorithm = collisionAlgorithmSelector->createCollisionAlgorithm(body1, body1->getShape(), body2, body2->getShape());

			overlappingPair->setCollisionAlgorithm(collisionAlgorithm);
		}

		return collisionAlgorithm;
	}

	void NarrowPhaseManager::processPredictiveContacts(float dt, std::vector<ManifoldResult> &manifoldResults)
	{
		for (auto workBody : bodyManager->getWorkBodies())
		{
			WorkRigidBody *body = WorkRigidBody::upCast(workBody);
			if(body!=nullptr && body->isActive()){
				const PhysicsTransform &currentTransform = body->getPhysicsTransform();
				PhysicsTransform newTransform = body->getPhysicsTransform().integrate(body->getLinearVelocity(), body->getAngularVelocity(), dt);

				float ccdMotionThreshold = body->getCcdMotionThreshold();
				float motion = currentTransform.getPosition().vector(newTransform.getPosition()).length();

				if(motion > ccdMotionThreshold)
				{
					handleContinuousCollision(body, currentTransform, newTransform, manifoldResults);
				}
			}
		}
	}

	void NarrowPhaseManager::handleContinuousCollision(AbstractWorkBody *body, const PhysicsTransform &from, const PhysicsTransform &to, std::vector<ManifoldResult> &manifoldResults)
	{
		std::vector<AbstractWorkBody *> bodiesAABBoxHitBody = broadPhaseManager->bodyTest(body, from, to);
		if(!bodiesAABBoxHitBody.empty())
		{
			ccd_set ccdResults;

			const CollisionShape3D *bodyShape = body->getShape();
			if(bodyShape->getShapeType()==CollisionShape3D::COMPOUND_SHAPE)
			{
				const auto *compoundShape = dynamic_cast<const CollisionCompoundShape *>(bodyShape);
				const std::vector<std::shared_ptr<const LocalizedCollisionShape>> & localizedShapes = compoundShape->getLocalizedShapes();
				for(const auto &localizedShape : localizedShapes)
				{
					TemporalObject temporalObject(localizedShape->shape.get(), from * localizedShape->transform, to * localizedShape->transform);
					ccd_set localizedShapeCcdResults = continuousCollisionTest(temporalObject, bodiesAABBoxHitBody);

					for(auto &localizedShapeCcdResult : localizedShapeCcdResults)
					{
						ccdResults.insert(localizedShapeCcdResult);
					}
				}
			}else
			{
				TemporalObject temporalObject(body->getShape(), from, to);
				ccdResults = continuousCollisionTest(temporalObject, bodiesAABBoxHitBody);
			}

			if(!ccdResults.empty())
			{
				std::shared_ptr<ContinuousCollisionResult<float>> firstCCDResult = *ccdResults.begin();

				Vector3<float> distanceVector = from.getPosition().vector(to.getPosition()) * firstCCDResult->getTimeToHit();
				float depth = distanceVector.dotProduct(-firstCCDResult->getNormalFromObject2());
				const Point3<float> &hitPointOnObject2 = firstCCDResult->getHitPointOnObject2();
				const Vector3<float> &normalFromObject2 = firstCCDResult->getNormalFromObject2();

				ManifoldResult manifoldResult(body, firstCCDResult->getBody2());
				manifoldResult.addContactPoint(normalFromObject2, hitPointOnObject2, depth, true);

				manifoldResults.push_back(manifoldResult);
			}
		}
	}

	ccd_set NarrowPhaseManager::continuousCollisionTest(const TemporalObject &temporalObject1, const std::vector<AbstractWorkBody *> &bodiesAABBoxHit) const
	{
		ccd_set continuousCollisionResults;

		for(auto bodyAABBoxHit : bodiesAABBoxHit)
		{
			const CollisionShape3D *bodyShape = bodyAABBoxHit->getShape();
			if(bodyShape->getShapeType()==CollisionShape3D::COMPOUND_SHAPE)
			{
				const auto *compoundShape = dynamic_cast<const CollisionCompoundShape *>(bodyShape);
				const std::vector<std::shared_ptr<const LocalizedCollisionShape>> &localizedShapes = compoundShape->getLocalizedShapes();
				for(const auto &localizedShape : localizedShapes)
				{
					PhysicsTransform fromToObject2 = bodyAABBoxHit->getPhysicsTransform() * localizedShape->transform;
					TemporalObject temporalObject2(localizedShape->shape.get(), fromToObject2, fromToObject2);

					continuousCollissionTest(temporalObject1, temporalObject2, bodyAABBoxHit, continuousCollisionResults);
				}
			}else
			{
				PhysicsTransform fromToObject2 = bodyAABBoxHit->getPhysicsTransform();
				TemporalObject temporalObject2(bodyShape, fromToObject2, fromToObject2);

				continuousCollissionTest(temporalObject1, temporalObject2, bodyAABBoxHit, continuousCollisionResults);
			}
		}

		return continuousCollisionResults;
	}

	/**
	 * @param continuousCollisionResults [OUT] In case of collision detected: continuous collision result will be updated with collision details
	 */
	void NarrowPhaseManager::continuousCollissionTest(const TemporalObject &temporalObject1, const TemporalObject &temporalObject2,
			AbstractWorkBody *body2, ccd_set &continuousCollisionResults) const
	{
		std::shared_ptr<ContinuousCollisionResult<float>> continuousCollisioncResult = gjkContinuousCollisionAlgorithm
				.calculateTimeOfImpact(temporalObject1, temporalObject2, body2);

		if(continuousCollisioncResult)
		{
			continuousCollisionResults.insert(continuousCollisioncResult);
		}
	}

	ccd_set NarrowPhaseManager::rayTest(const Ray<float> &ray, const std::vector<AbstractWorkBody *> &bodiesAABBoxHitRay) const
	{
		CollisionSphereShape pointShape(0.0f);
		PhysicsTransform from = PhysicsTransform(ray.getOrigin());
		PhysicsTransform to = PhysicsTransform(ray.computeTo());
		TemporalObject rayCastObject(&pointShape, from, to);

		return continuousCollisionTest(rayCastObject, bodiesAABBoxHitRay);
	}

}
