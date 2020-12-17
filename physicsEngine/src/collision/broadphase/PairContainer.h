#ifndef URCHINENGINE_PAIRCONTAINER_H
#define URCHINENGINE_PAIRCONTAINER_H

#include "body/model/AbstractBody.h"
#include "collision/OverlappingPair.h"

namespace urchin {

    /**
    * Broad phase overlapping pairs container interface
    */
    class PairContainer {
        public:
            virtual ~PairContainer() = default;

            virtual void addOverlappingPair(AbstractBody*, AbstractBody*) = 0;
            virtual void removeOverlappingPair(AbstractBody*, AbstractBody*) = 0;
            virtual void removeOverlappingPairs(AbstractBody*) = 0;

            virtual const std::vector<OverlappingPair*>& getOverlappingPairs() const = 0;
            virtual std::vector<OverlappingPair> retrieveCopyOverlappingPairs() const = 0;
    };

}

#endif
