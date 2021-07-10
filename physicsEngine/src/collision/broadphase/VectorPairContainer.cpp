#include <algorithm>

#include <collision/broadphase/VectorPairContainer.h>

namespace urchin {

    void VectorPairContainer::addOverlappingPair(AbstractBody* body1, AbstractBody* body2) {
        uint_fast64_t bodiesId = OverlappingPair::computeBodiesId(body1, body2);

        bool found = false;
        for (auto& overlappingPair : overlappingPairs) {
            if (overlappingPair->getBodiesId() == bodiesId) {
                found = true;
                break;
            }
        }

        if (!found) { //pair doesn't exist: we create it
            overlappingPairs.push_back(std::make_unique<OverlappingPair>(body1, body2, bodiesId));
        }
    }

    void VectorPairContainer::removeOverlappingPair(AbstractBody* body1, AbstractBody* body2) {
        uint_fast64_t bodiesId = OverlappingPair::computeBodiesId(body1, body2);
        for (auto it = overlappingPairs.begin(); it != overlappingPairs.end(); ++it) {
            if ((*it)->getBodiesId() == bodiesId) {
                VectorUtil::erase(overlappingPairs, it);
                break;
            }
        }
    }

    void VectorPairContainer::removeOverlappingPairs(AbstractBody* body) {
        auto it = overlappingPairs.begin();
        while (it != overlappingPairs.end()) {
            auto& pair = *it;
            if (pair->getBody1() == body || pair->getBody2() == body) {
                it = overlappingPairs.erase(it);
            } else {
                ++it;
            }
        }
    }

    const std::vector<std::unique_ptr<OverlappingPair>>& VectorPairContainer::getOverlappingPairs() const {
        return overlappingPairs;
    }

    std::vector<OverlappingPair> VectorPairContainer::retrieveCopyOverlappingPairs() const {
        throw std::runtime_error("Not implemented: use 'getOverlappingPairs' method");
    }
}
