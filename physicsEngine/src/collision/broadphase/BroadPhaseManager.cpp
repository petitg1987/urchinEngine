#include "collision/broadphase/BroadPhaseManager.h"
#include "body/work/AbstractWorkBody.h"
#include "body/work/WorkGhostBody.h"

namespace urchin
{

	BroadPhaseManager::BroadPhaseManager(BodyManager *bodyManager)
	{
		sweepAndPrune = new SweepAndPrune();

		bodyManager->addObserver(this, BodyManager::ADD_WORK_BODY);
		bodyManager->addObserver(this, BodyManager::REMOVE_WORK_BODY);
	}

	BroadPhaseManager::~BroadPhaseManager()
	{
		delete sweepAndPrune;
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
		sweepAndPrune->addBody(body, alternativePairContainer);
	}

	void BroadPhaseManager::removeBody(AbstractWorkBody *body)
	{
		sweepAndPrune->removeBody(body);
	}

	const std::vector<OverlappingPair *> &BroadPhaseManager::computeOverlappingPairs()
	{
		sweepAndPrune->updateBodies();
		return sweepAndPrune->getOverlappingPairs();
	}

}
