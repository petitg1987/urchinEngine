#ifndef URCHINENGINE_SYNCVECTORPAIRCONTAINER_H
#define URCHINENGINE_SYNCVECTORPAIRCONTAINER_H

#include <mutex>

#include "collision/broadphase/VectorPairContainer.h"

namespace urchin {

    /**
     * Thread safe pair container. Pair can be added/removed by physics thread while pairs are read from another thread.
     */
    class SyncVectorPairContainer : public VectorPairContainer {
        public:
            ~SyncVectorPairContainer() override = default;

            void addOverlappingPair(AbstractWorkBody *, AbstractWorkBody*) override;
            void removeOverlappingPair(AbstractWorkBody *, AbstractWorkBody*) override;
            void removeOverlappingPairs(AbstractWorkBody *) override;

            const std::vector<OverlappingPair *> &getOverlappingPairs() const override;
            std::vector<OverlappingPair> retrieveCopyOverlappingPairs() const override;

        private:
            mutable std::mutex pairMutex;
    };

}

#endif
