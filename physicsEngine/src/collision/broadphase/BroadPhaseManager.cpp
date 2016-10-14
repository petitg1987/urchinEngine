#include "collision/broadphase/BroadPhaseManager.h"
#include "body/work/AbstractWorkBody.h"
#include "body/work/WorkGhostBody.h"
#include "collision/broadphase/sweepandprune/SweepAndPrune.h"
#include "collision/broadphase/aabbtree/AABBTreeAlgorithm.h"

namespace urchin
{

	BroadPhaseManager::BroadPhaseManager(BodyManager *bodyManager)
	{
		broadPhaseAlgorithm = new AABBTreeAlgorithm();

		bodyManager->addObserver(this, BodyManager::ADD_WORK_BODY);
		bodyManager->addObserver(this, BodyManager::REMOVE_WORK_BODY);
	}

	BroadPhaseManager::~BroadPhaseManager()
	{
		delete broadPhaseAlgorithm;
	}

	void BroadPhaseManager::notify(Observable *observable, int notificationType)
	{
		BodyManager *bodyManager = dynamic_cast<BodyManager *>(observable);
		if(bodyManager)
		{
			AbstractWorkBody *updatedBody = bodyManager->getLastUpdatedWorkBody();
			switch(notificationType)
			{
				case BodyManager::ADD_WORK_BODY:
				{
					addBody(updatedBody, nullptr);
					break;
				}
				case BodyManager::REMOVE_WORK_BODY:
				{
					removeBody(updatedBody);
					break;
				}
			}
		}
	}

	/**
	 * @param alternativePairContainer Alternative pair container to default one
	 */
	void BroadPhaseManager::addBody(AbstractWorkBody *body, PairContainer *alternativePairContainer)
	{
		broadPhaseAlgorithm->addBody(body, alternativePairContainer);
	}

	void BroadPhaseManager::removeBody(AbstractWorkBody *body)
	{
		broadPhaseAlgorithm->removeBody(body);
	}

	const std::vector<OverlappingPair *> &BroadPhaseManager::computeOverlappingPairs()
	{
		broadPhaseAlgorithm->updateBodies();
		return broadPhaseAlgorithm->getOverlappingPairs();
	}

	std::vector<AbstractWorkBody *> BroadPhaseManager::rayTest(const Ray<float> &ray) const
	{
		return broadPhaseAlgorithm->rayTest(ray);
	}

	/**
	 * Enlarge each box of a specified size and process a classical ray test. This method provide similar result to a OBB test but with better performance.
	 * @param enlargeBoxHalfSize Specify the size of the enlargement. A size of 0.5 in X axis will enlarge the box from 1.0 (0.5 on left and 0.5 on right).
	 */
	std::vector<AbstractWorkBody *> BroadPhaseManager::enlargedRayTest(const Ray<float> &ray, const Vector3<float> &enlargeBoxHalfSize) const
	{
		return broadPhaseAlgorithm->enlargedRayTest(ray, enlargeBoxHalfSize);
	}

}
