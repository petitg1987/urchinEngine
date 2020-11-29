#ifndef URCHINENGINE_PAIRCONTAINER_H
#define URCHINENGINE_PAIRCONTAINER_H

#include "body/work/AbstractWorkBody.h"
#include "collision/OverlappingPair.h"

namespace urchin {

    /**
    * Broad phase overlapping pairs container interface
    */
    class PairContainer {
        public:
            virtual ~PairContainer() = default;

            virtual void addOverlappingPair(AbstractWorkBody*, AbstractWorkBody*) = 0;
            virtual void removeOverlappingPair(AbstractWorkBody*, AbstractWorkBody*) = 0;
            virtual void removeOverlappingPairs(AbstractWorkBody*) = 0;

            virtual const std::vector<OverlappingPair *>& getOverlappingPairs() const = 0;
            virtual std::vector<OverlappingPair> retrieveCopyOverlappingPairs() const = 0;
    };

}

#endif
