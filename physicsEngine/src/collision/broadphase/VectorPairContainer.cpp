#include <algorithm>

#include "VectorPairContainer.h"

namespace urchin {

    VectorPairContainer::~VectorPairContainer() {
        for (auto& overlappingPair : overlappingPairs) {
            delete overlappingPair;
        }
    }

    void VectorPairContainer::addOverlappingPair(AbstractWorkBody* body1, AbstractWorkBody* body2) {
        uint_fast64_t bodiesId = OverlappingPair::computeBodiesId(body1, body2);

        bool found = false;
        for (auto& overlappingPair : overlappingPairs) {
            if (overlappingPair->getBodiesId() == bodiesId) {
                found = true;
                break;
            }
        }

        if (!found) { //pair doesn't exist: we create it
            overlappingPairs.push_back(new OverlappingPair(body1, body2, bodiesId));
        }
    }

    void VectorPairContainer::removeOverlappingPair(AbstractWorkBody* body1, AbstractWorkBody* body2) {
        uint_fast64_t bodiesId = OverlappingPair::computeBodiesId(body1, body2);
        for (auto it = overlappingPairs.begin(); it != overlappingPairs.end(); ++it) {
            if ((*it)->getBodiesId() == bodiesId) {
                delete *it;
                VectorEraser::erase(overlappingPairs, it);
                break;
            }
        }
    }

    void VectorPairContainer::removeOverlappingPairs(AbstractWorkBody* body) {
        auto it = overlappingPairs.begin();
        while (it != overlappingPairs.end()) {
            OverlappingPair* pair = *it;

            if (pair->getBody1() == body || pair->getBody2() == body) {
                it = overlappingPairs.erase(it);
                delete pair;
            } else {
                ++it;
            }
        }
    }

    const std::vector<OverlappingPair*>& VectorPairContainer::getOverlappingPairs() const {
        return overlappingPairs;
    }

    std::vector<OverlappingPair> VectorPairContainer::retrieveCopyOverlappingPairs() const {
        throw std::runtime_error("Not implemented: use 'getOverlappingPairs' method");
    }
}
