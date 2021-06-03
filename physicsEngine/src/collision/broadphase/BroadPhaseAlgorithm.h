#ifndef URCHINENGINE_BROADPHASEALGORITHM_H
#define URCHINENGINE_BROADPHASEALGORITHM_H

#include <vector>
#include <UrchinCommon.h>

#include <body/model/AbstractBody.h>
#include <collision/OverlappingPair.h>
#include <collision/broadphase/PairContainer.h>

namespace urchin {

    class BroadPhaseAlgorithm {
        public:
            virtual ~BroadPhaseAlgorithm() = default;

            virtual void addBody(AbstractBody*, PairContainer*) = 0;
            virtual void removeBody(AbstractBody*) = 0;
            virtual void updateBodies() = 0;

            virtual const std::vector<OverlappingPair*>& getOverlappingPairs() const = 0;

            virtual std::vector<AbstractBody*> rayTest(const Ray<float>&) const = 0;
            virtual std::vector<AbstractBody*> bodyTest(AbstractBody*, const PhysicsTransform&, const PhysicsTransform&) const = 0;
    };

}

#endif
