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

}
