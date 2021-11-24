#include <collision/broadphase/SyncVectorPairContainer.h>

namespace urchin {

    void SyncVectorPairContainer::addOverlappingPair(std::shared_ptr<AbstractBody> body1, std::shared_ptr<AbstractBody> body2) {
        std::scoped_lock<std::mutex> lock(pairMutex);
        VectorPairContainer::addOverlappingPair(std::move(body1), std::move(body2));
    }

    void SyncVectorPairContainer::removeOverlappingPair(AbstractBody& body1, AbstractBody& body2) {
        std::scoped_lock<std::mutex> lock(pairMutex);
        VectorPairContainer::removeOverlappingPair(body1, body2);
    }

    void SyncVectorPairContainer::removeOverlappingPairs(AbstractBody& body) {
        std::scoped_lock<std::mutex> lock(pairMutex);
        VectorPairContainer::removeOverlappingPairs(body);
    }

    const std::vector<std::unique_ptr<OverlappingPair>>& SyncVectorPairContainer::getOverlappingPairs() const {
        throw std::runtime_error("Cannot retrieve overlapping pairs reference on a thread safe container: use 'retrieveCopyOverlappingPairs' method");
    }

    /**
     * @param copiedOverlappingPairs [out] Copy of overlapping pairs of the container
     */
    void SyncVectorPairContainer::retrieveCopyOverlappingPairs(std::vector<OverlappingPair>& copiedOverlappingPairs) const {
        //This method can be called by several threads and therefore must return a real copy of overlapping pairs.
        // - Returning a reference to a class attribute will not work as vector methods (e.g.: clear()) could be called by two different threads later
        // - Using a 'thread_local std::vector<OverlappingPair>' and return the reference seems to not work. It generates invalid read/write in Valgrind !

        std::scoped_lock<std::mutex> lock(pairMutex);
        copiedOverlappingPairs.reserve(overlappingPairs.size());
        for (const auto& overlappingPair : overlappingPairs) {
            copiedOverlappingPairs.emplace_back(*overlappingPair);
        }
    }

}
