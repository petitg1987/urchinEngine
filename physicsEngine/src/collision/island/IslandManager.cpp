#include "collision/island/IslandManager.h"

namespace urchin
{

	IslandManager::IslandManager(const BodyManager *bodyManager) :
		bodyManager(bodyManager),
		squaredLinearSleepingThreshold(ConfigService::instance()->getFloatValue("island.linearSleepingThreshold") * ConfigService::instance()->getFloatValue("island.linearSleepingThreshold")),
		squaredAngularSleepingThreshold(ConfigService::instance()->getFloatValue("island.angularSleepingThreshold") * ConfigService::instance()->getFloatValue("island.angularSleepingThreshold"))
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
	void IslandManager::refreshBodyActiveState(const std::vector<ManifoldResult> &manifoldResults)
	{
		buildIslands(manifoldResults);
		const std::vector<IslandElementLink> &islandElementsLink = islandContainer.retrieveSortedIslandElements();

		#ifdef _DEBUG
			//printIslands(islandElementsLink);
		#endif

		unsigned int i=0;
		while(islandElementsLink.size()>i)
		{ //loop on islands
			unsigned int startElementIndex = i;
			unsigned int nbElements = computeNumberElements(islandElementsLink, startElementIndex);

			bool islandLinkedToStaticElement = false;
			bool islandBodiesCanSleep = true;
			for(unsigned int j=0; j<nbElements; ++j)
			{ //loop on elements of the island
				WorkRigidBody *body = static_cast<WorkRigidBody *>(islandElementsLink[startElementIndex+j].element);
				if(isBodyMoving(body))
				{
					islandBodiesCanSleep = false;
					break;
				}

				islandLinkedToStaticElement = islandLinkedToStaticElement || islandElementsLink[startElementIndex+j].linkedToStaticElement;
			}
			islandBodiesCanSleep = islandBodiesCanSleep && islandLinkedToStaticElement; //one element of the island must be in contact with a static element to sleep the island

			for(unsigned int j=0; j<nbElements; ++j)
			{ //loop on elements of the island
				WorkRigidBody *body = static_cast<WorkRigidBody *>(islandElementsLink[startElementIndex+j].element);
				bool bodyActiveState = !islandBodiesCanSleep;
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

	void IslandManager::buildIslands(const std::vector<ManifoldResult> &manifoldResults)
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
		for(const auto &manifoldResult : manifoldResults)
		{
			if(manifoldResult.getNumContactPoints() > 0)
			{
				AbstractWorkBody *body1 = manifoldResult.getBody1();
				AbstractWorkBody *body2 = manifoldResult.getBody2();

				if(!body1->isStatic() && !body2->isStatic())
				{
					islandContainer.mergeIsland(body1, body2);
				}else if(!body1->isStatic() && body2->isStatic())
				{
					islandContainer.linkToStaticElement(body1);
				}else if(!body2->isStatic() && body1->isStatic())
				{
					islandContainer.linkToStaticElement(body2);
				}
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
		if(body->getLinearVelocity().squareLength() < squaredLinearSleepingThreshold
				&& body->getAngularVelocity().squareLength() < squaredAngularSleepingThreshold)
		{
			return false;
		}

		return true;
	}

	#ifdef _DEBUG
		void IslandManager::printIslands(const std::vector<IslandElementLink> &islandElementsLink)
		{
			unsigned int islandId = 0;
			unsigned int i=0;
			while(islandElementsLink.size()>i)
			{ //loop on islands
				unsigned int startElementIndex = i;
				unsigned int nbElements = computeNumberElements(islandElementsLink, startElementIndex);

				std::cout<<"Island "<<islandId<<":"<<std::endl;

				for(unsigned int j=0; j<nbElements; ++j)
				{ //loop on elements of the island
					WorkRigidBody *body = static_cast<WorkRigidBody *>(islandElementsLink[startElementIndex+j].element);
					std::cout<<"  - Body: "<<body->getId()<<" (moving: "<<isBodyMoving(body)<<", active: "<<body->isActive()<<")"<<std::endl;
				}

				i += nbElements;
				islandId++;
			}

			std::cout<<std::endl;
		}
	#endif

}
