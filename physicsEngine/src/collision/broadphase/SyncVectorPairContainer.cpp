#include "SyncVectorPairContainer.h"

namespace urchin {

    void SyncVectorPairContainer::addOverlappingPair(AbstractWorkBody *body1, AbstractWorkBody *body2) {
        std::lock_guard<std::mutex> lock(pairMutex);

        VectorPairContainer::addOverlappingPair(body1, body2);
    }

    void SyncVectorPairContainer::removeOverlappingPair(AbstractWorkBody *body1, AbstractWorkBody *body2) {
        std::lock_guard<std::mutex> lock(pairMutex);

        VectorPairContainer::removeOverlappingPair(body1, body2);
    }

    void SyncVectorPairContainer::removeOverlappingPairs(AbstractWorkBody *body) {
        std::lock_guard<std::mutex> lock(pairMutex);

        VectorPairContainer::removeOverlappingPairs(body);
    }

    const std::vector<OverlappingPair *> &SyncVectorPairContainer::getOverlappingPairs() const {
        throw std::runtime_error("Cannot retrieve overlapping pairs reference on a thread safe container: use 'retrieveCopyOverlappingPairs' method");
    }

    std::vector<OverlappingPair> SyncVectorPairContainer::retrieveCopyOverlappingPairs() const {
        //This method can be called by several threads and therefore must return a real copy of overlapping pairs.
        // - Returning a reference to a class attribute will not work as vector methods (e.g.: clear()) could be called by two different threads later
        // - Using a 'thread_local std::vector<OverlappingPair>' and return the reference seems to not work. It generates invalid read/write in Valgrind !

        std::lock_guard<std::mutex> lock(pairMutex);

        std::vector<OverlappingPair> copiedOverlappingPairs;
        copiedOverlappingPairs.reserve(overlappingPairs.size());
        for(const auto &overlappingPair : overlappingPairs) {
            copiedOverlappingPairs.emplace_back(OverlappingPair(*overlappingPair));
        }
        return copiedOverlappingPairs;
    }

}
