#include <algorithm>
#include <cassert>

#include "collision/island/IslandContainer.h"

namespace urchin
{

	IslandContainer::IslandContainer()
	{
		#ifdef _DEBUG
			containerSorted = false;
		#endif
	}

	IslandContainer::~IslandContainer()
	{

	}

	/**
	 * Reset the container of island elements. Create islands of one element for each island elements asked.
	 */
	void IslandContainer::reset(const std::vector<IslandElement *> &islandElements)
	{
		#ifdef _DEBUG
			containerSorted = false;
		#endif

		islandElementsLink.clear();
		islandElementsLink.resize(islandElements.size());

		for(unsigned int i=0; i<islandElements.size(); ++i)
		{
			islandElements[i]->setIslandElementId(i);

			islandElementsLink[i].element = islandElements[i];
			islandElementsLink[i].islandIdRef = i;
		}
	}

	void IslandContainer::mergeIsland(IslandElement *element1, IslandElement *element2)
	{
		#ifdef _DEBUG
			assert(!containerSorted);
		#endif

		unsigned int islandId1 = findIslandId(element1->getIslandElementId());
		unsigned int islandId2 = findIslandId(element2->getIslandElementId());

		if(islandId1==islandId2)
		{ //elements are already in the same island
			return;
		}

		islandElementsLink[islandId1].islandIdRef = islandId2;
	}

	/**
	 * Sorts the islands by ID and returns them.
	 * Once the islands sorted, the container is not usable anymore and need to be reset.
	 */
	const std::vector<IslandElementLink> &IslandContainer::retrieveSortedIslandElements()
	{
		//store directly island ID on islandIdRef instead of reference
		for(unsigned int i=0; i<islandElementsLink.size(); ++i)
		{
			islandElementsLink[i].islandIdRef = findIslandId(i);
		}

		std::sort(islandElementsLink.begin(), islandElementsLink.end(), IslandElementLinkSortPredicate());

		#ifdef _DEBUG
			containerSorted = true;
		#endif

		return islandElementsLink;
	}

	unsigned int IslandContainer::findIslandId(unsigned int elementRef) const
	{
		while(elementRef!=islandElementsLink[elementRef].islandIdRef)
		{
			elementRef = islandElementsLink[elementRef].islandIdRef;
		}

		return elementRef;
	}

	unsigned int IslandContainer::getSize() const
	{
		return islandElementsLink.size();
	}

}
