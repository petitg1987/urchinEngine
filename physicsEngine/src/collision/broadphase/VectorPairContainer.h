#ifndef ENGINE_VECTORPAIRCONTAINER_H
#define ENGINE_VECTORPAIRCONTAINER_H

#include <vector>

#include "collision/OverlappingPair.h"
#include "PairContainer.h"

namespace urchin
{

	/**
	* Overlapping pair manager using a std::vector. Vectors have very high performance
	* to looping over. Add and remove operations have also good performance when number
	* of elements are limited (< 1000) due to little cache miss.
	*/
	class VectorPairContainer : public PairContainer
	{
		public:
			VectorPairContainer();
			~VectorPairContainer();

			void addOverlappingPair(AbstractWorkBody *, AbstractWorkBody *);
			void removeOverlappingPair(AbstractWorkBody *, AbstractWorkBody *);
			void removeOverlappingPairs(AbstractWorkBody *);

			const std::vector<OverlappingPair *> &getOverlappingPairs() const;

		private:
			std::vector<OverlappingPair *> overlappingPairs;
	};

}

#endif
