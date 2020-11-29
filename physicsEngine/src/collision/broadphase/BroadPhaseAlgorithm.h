#ifndef URCHINENGINE_BROADPHASEALGORITHM_H
#define URCHINENGINE_BROADPHASEALGORITHM_H

#include <vector>
#include "UrchinCommon.h"

#include "body/work/AbstractWorkBody.h"
#include "collision/OverlappingPair.h"
#include "collision/broadphase/PairContainer.h"

namespace urchin {

    class BroadPhaseAlgorithm {
        public:
            virtual ~BroadPhaseAlgorithm() = default;

            virtual void addBody(AbstractWorkBody*, PairContainer*) = 0;
            virtual void removeBody(AbstractWorkBody*) = 0;
            virtual void updateBodies() = 0;

            virtual const std::vector<OverlappingPair *> &getOverlappingPairs() const = 0;

            virtual std::vector<AbstractWorkBody *> rayTest(const Ray<float>&) const = 0;
            virtual std::vector<AbstractWorkBody *> bodyTest(AbstractWorkBody*, const PhysicsTransform&, const PhysicsTransform&) const = 0;
    };

}

#endif
