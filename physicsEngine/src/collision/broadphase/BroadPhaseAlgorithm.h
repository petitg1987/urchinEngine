#pragma once

#include <vector>
#include <UrchinCommon.h>

#include <body/model/AbstractBody.h>
#include <collision/OverlappingPair.h>

namespace urchin {

    class BroadPhaseAlgorithm {
        public:
            virtual ~BroadPhaseAlgorithm() = default;

            virtual void addBody(const std::shared_ptr<AbstractBody>&) = 0;
            virtual void removeBody(const AbstractBody&) = 0;
            virtual void updateBodies() = 0;

            virtual const std::vector<std::unique_ptr<OverlappingPair>>& getOverlappingPairs() const = 0;

            virtual std::vector<std::shared_ptr<AbstractBody>> rayTest(const Ray<float>&) const = 0;
            virtual std::vector<std::shared_ptr<AbstractBody>> bodyTest(const AbstractBody&, const PhysicsTransform&, const PhysicsTransform&) const = 0;
    };

}
