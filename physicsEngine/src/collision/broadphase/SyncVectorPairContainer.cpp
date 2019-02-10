#include "SyncVectorPairContainer.h"

namespace urchin
{

    void SyncVectorPairContainer::addOverlappingPair(AbstractWorkBody *body1, AbstractWorkBody *body2)
    {
        std::lock_guard<std::mutex> lock(pairMutex);

        VectorPairContainer::addOverlappingPair(body1, body2);
    }

    void SyncVectorPairContainer::removeOverlappingPair(AbstractWorkBody *body1, AbstractWorkBody *body2)
    {
        std::lock_guard<std::mutex> lock(pairMutex);

        VectorPairContainer::removeOverlappingPair(body1, body2);
    }

    void SyncVectorPairContainer::removeOverlappingPairs(AbstractWorkBody *body)
    {
        std::lock_guard<std::mutex> lock(pairMutex);

        VectorPairContainer::removeOverlappingPairs(body);
    }

    const std::vector<OverlappingPair *> &SyncVectorPairContainer::getOverlappingPairs() const
    {
        throw std::runtime_error("Cannot retrieve overlapping pairs reference on a thread safe container");
    }

    std::vector<OverlappingPair> &SyncVectorPairContainer::retrieveCopyOverlappingPairs() const
    {
        std::lock_guard<std::mutex> lock(pairMutex);

        return VectorPairContainer::retrieveCopyOverlappingPairs();
    }

}
