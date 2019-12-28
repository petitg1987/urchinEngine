#ifndef URCHINENGINE_VECTORPAIRCONTAINER_H
#define URCHINENGINE_VECTORPAIRCONTAINER_H

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
			~VectorPairContainer() override;

            void addOverlappingPair(AbstractWorkBody *, AbstractWorkBody *) override;
            void removeOverlappingPair(AbstractWorkBody *, AbstractWorkBody *) override;
            void removeOverlappingPairs(AbstractWorkBody *) override;

            const std::vector<OverlappingPair *> &getOverlappingPairs() const override;
            std::vector<OverlappingPair> retrieveCopyOverlappingPairs() const override;

		protected:
			std::vector<OverlappingPair *> overlappingPairs;
	};

}

#endif
