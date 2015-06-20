#ifndef ENGINE_PAIRCONTAINER_H
#define ENGINE_PAIRCONTAINER_H

#include "collision/OverlappingPair.h"

namespace urchin
{

	/**
	* Broad phase overlapping pairs container interface
	*/
	class PairContainer
	{
		public:
			PairContainer();
			virtual ~PairContainer();

			virtual void addOverlappingPair(AbstractWorkBody *, AbstractWorkBody *) = 0;
			virtual void removeOverlappingPair(AbstractWorkBody *, AbstractWorkBody *) = 0;
			virtual void removeOverlappingPairs(AbstractWorkBody *) = 0;

			virtual const std::vector<OverlappingPair *> &getOverlappingPairs() const = 0;
	};

}

#endif
