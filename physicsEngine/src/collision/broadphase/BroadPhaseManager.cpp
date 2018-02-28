#include "collision/broadphase/BroadPhaseManager.h"
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
		auto *bodyManager = dynamic_cast<BodyManager *>(observable);
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
				default:
					break;
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
		ScopeProfiler profiler("physics", "computeOverlappingPairs");

		broadPhaseAlgorithm->updateBodies();
		return broadPhaseAlgorithm->getOverlappingPairs();
	}

	std::vector<AbstractWorkBody *> BroadPhaseManager::rayTest(const Ray<float> &ray) const
	{
		return broadPhaseAlgorithm->rayTest(ray);
	}

	std::vector<AbstractWorkBody *> BroadPhaseManager::bodyTest(const AbstractWorkBody *body, const PhysicsTransform &from, const PhysicsTransform &to) const
	{
		return broadPhaseAlgorithm->bodyTest(body, from, to);
	}

}
