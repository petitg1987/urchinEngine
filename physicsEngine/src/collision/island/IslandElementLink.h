#ifndef ENGINE_ISLANDELEMENTLINK_H
#define ENGINE_ISLANDELEMENTLINK_H

#include "collision/island/IslandElement.h"

namespace urchin
{

	/**
	* Allow to make the link between an island element and the island ID.
	*/
	struct IslandElementLink
	{
		IslandElementLink();
		~IslandElementLink();

		IslandElement *element; //reference to the element

		unsigned int islandIdRef; //Reference to the next island element. If it references itself: it's the island id.
	};

	struct IslandElementLinkSortPredicate
	{
		bool operator() (const IslandElementLink &elem1, const IslandElementLink &elem2) const
		{
			return elem1.islandIdRef < elem2.islandIdRef;
		}
	};

}

#endif
