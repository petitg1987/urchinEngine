#include "collision/island/IslandManager.h"

namespace urchin
{

	IslandManager::IslandManager(const BodyManager *bodyManager) :
		bodyManager(bodyManager),
		linearSleepingThreshold(ConfigService::instance()->getFloatValue("island.immutable.linearSleepingThreshold")),
		angularSleepingThreshold(ConfigService::instance()->getFloatValue("island.immutable.angularSleepingThreshold"))
	{

	}

	IslandManager::~IslandManager()
	{

	}

	/**
	 * Refresh body active state. If all bodies of an island can sleep, we set their status to inactive.
	 * If one body of the island cannot sleep, we set their status to active.
	 * @param overlappingPairs Overlapping pairs of broad phase used to determine the islands
	 */
	void IslandManager::refreshBodyActiveState(const std::vector<OverlappingPair *> &overlappingPairs)
	{
		buildIslands(overlappingPairs);
		const std::vector<IslandElementLink> &islandElementsLink = islandContainer.retrieveSortedIslandElements();

		unsigned int i=0;
		while(islandElementsLink.size()>i)
		{ //loop on islands
			unsigned int startElementIndex = i;
			unsigned int nbElements = computeNumberElements(islandElementsLink, startElementIndex);

			bool allBodiesCanSleep = true;
			for(unsigned int j=0; j<nbElements; ++j)
			{ //loop on elements of the island
				WorkRigidBody *body = static_cast<WorkRigidBody *>(islandElementsLink[startElementIndex+j].element);
				if(isBodyMoving(body))
				{
					allBodiesCanSleep = false;
					break;
				}
			}

			for(unsigned int j=0; j<nbElements; ++j)
			{ //loop on elements of the island
				WorkRigidBody *body = static_cast<WorkRigidBody *>(islandElementsLink[startElementIndex+j].element);
				bool bodyActiveState = !allBodiesCanSleep;
				if(body->isActive()!=bodyActiveState)
				{
					body->setIsActive(bodyActiveState);

					if(bodyActiveState)
					{
						body->setLinearVelocity(Vector3<float>(0.0, 0.0, 0.0));
						body->setAngularVelocity(Vector3<float>(0.0, 0.0, 0.0));
					}
				}
			}

			i += nbElements;
		}
	}

	void IslandManager::buildIslands(const std::vector<OverlappingPair *> &overlappingPairs)
	{
		//1. create an island for each body
		std::vector<IslandElement *> islandElements;
		islandElements.reserve(bodyManager->getWorkBodies().size());
		for(unsigned int i=0; i<bodyManager->getWorkBodies().size(); i++)
		{
			AbstractWorkBody *body = bodyManager->getWorkBodies()[i];
			if(!body->isStatic())
			{
				islandElements.push_back(body);
			}
		}
		islandContainer.reset(islandElements);

		//2. merge islands for bodies in contact
		for(std::vector<OverlappingPair *>::const_iterator it = overlappingPairs.begin(); it!=overlappingPairs.end(); ++it)
		{
			AbstractWorkBody *body1 = (*it)->getBody1();
			AbstractWorkBody *body2 = (*it)->getBody2();

			if(!body1->isStatic() && !body2->isStatic())
			{
				islandContainer.mergeIsland(body1, body2);
			}
		}
	}

	/**
	 * @return Number of element for island starting at 'startElementIndex'
	 */
	unsigned int IslandManager::computeNumberElements(const std::vector<IslandElementLink> &islandElementsLink, unsigned int startElementIndex) const
	{
		unsigned int islandId = islandElementsLink[startElementIndex].islandIdRef;
		unsigned int endElementIndex = startElementIndex+1;

		for(endElementIndex = startElementIndex; islandElementsLink.size()>endElementIndex && islandId==islandElementsLink[endElementIndex].islandIdRef; ++endElementIndex)
		{
			;
		}

		return endElementIndex - startElementIndex;
	}

	bool IslandManager::isBodyMoving(const WorkRigidBody *body) const
	{
		if(body->getLinearVelocity().squareLength() < linearSleepingThreshold*linearSleepingThreshold
				&& body->getAngularVelocity().squareLength() < angularSleepingThreshold*angularSleepingThreshold)
		{
			return false;
		}

		return true;
	}

}
