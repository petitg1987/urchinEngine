#include "collision/narrowphase/NarrowPhaseManager.h"
#include "collision/narrowphase/algorithm/raycast/RayCastObject.h"
#include "processable/raytest/RayTestSingleResult.h"
#include "shape/CollisionShape3D.h"
#include "shape/CollisionSphereShape.h"

namespace urchin
{

	NarrowPhaseManager::NarrowPhaseManager()
	{
		collisionAlgorithmSelector = new CollisionAlgorithmSelector();
		manifoldResults = new std::vector<ManifoldResult>();
	}

	NarrowPhaseManager::~NarrowPhaseManager()
	{
		manifoldResults->clear();
		delete manifoldResults;

		delete collisionAlgorithmSelector;
	}

	std::vector<ManifoldResult> *NarrowPhaseManager::process(const std::vector<OverlappingPair *> &overlappingPairs)
	{
		manifoldResults->clear();

		for(std::vector<OverlappingPair *>::const_iterator it = overlappingPairs.begin(); it!=overlappingPairs.end(); ++it)
		{
			AbstractWorkBody *body1 = (*it)->getBody1();
			AbstractWorkBody *body2 = (*it)->getBody2();

			if(body1->isActive() || body2->isActive())
			{
				std::shared_ptr<CollisionAlgorithm> collisionAlgorithm = retrieveCollisionAlgorithm(*it);
				const CollisionAlgorithm *const constCollisionAlgorithm = collisionAlgorithm.get();

				CollisionObjectWrapper collisionObject1(*body1->getShape(), body1->getPhysicsTransform());
				CollisionObjectWrapper collisionObject2(*body2->getShape(), body2->getPhysicsTransform());
				collisionAlgorithm->processCollisionAlgorithm(collisionObject1, collisionObject2, true);

				if(constCollisionAlgorithm->getManifoldResult().getNumContactPoints()!=0)
				{
					manifoldResults->push_back(constCollisionAlgorithm->getManifoldResult());
				}
			}
		}

		return manifoldResults;
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

	std::vector<std::shared_ptr<RayCastResult<double>>> NarrowPhaseManager::rayTest(const Ray<float> &ray, const std::vector<AbstractWorkBody *> &bodiesAABBoxHitRay) const
	{
		std::vector<std::shared_ptr<RayCastResult<double>>> rayCastResults;

		CollisionSphereShape pointShapeA(0.0);
		PhysicsTransform fromA = PhysicsTransform(ray.getOrigin());
		PhysicsTransform toA = PhysicsTransform(ray.computeTo());
		RayCastObject rayCastObjectA(&pointShapeA, fromA, toA);

		for(auto bodyAABBoxHitRay : bodiesAABBoxHitRay)
		{
			PhysicsTransform fromToB = bodyAABBoxHitRay->getPhysicsTransform();
			RayCastObject rayCastObjectB(bodyAABBoxHitRay->getShape(), fromToB, fromToB);

			rayCastResults.push_back(gjkRayCastAlgorithm.calculateTimeOfImpact(rayCastObjectA, rayCastObjectB));
		}

		return rayCastResults;
	}

}
