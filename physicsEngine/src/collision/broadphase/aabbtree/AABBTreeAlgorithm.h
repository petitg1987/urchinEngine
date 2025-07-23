#pragma once

#include <UrchinCommon.h>

#include "body/model/AbstractBody.h"
#include "collision/OverlappingPair.h"
#include "collision/broadphase/BroadPhaseAlgorithm.h"
#include "collision/broadphase/aabbtree/BodyAABBTree.h"

namespace urchin {

    class AABBTreeAlgorithm final : public BroadPhaseAlgorithm {
        public:
            AABBTreeAlgorithm();

            void addBody(const std::shared_ptr<AbstractBody>&) override;
            void removeBody(const AbstractBody&) override;
            void updateBodies() override;

            const std::vector<std::unique_ptr<OverlappingPair>>& getOverlappingPairs() const override;

            void rayTest(const Ray<float>&, std::vector<std::shared_ptr<AbstractBody>>&) const override;
            void bodyTest(const AbstractBody&, const PhysicsTransform&, const PhysicsTransform&, std::vector<std::shared_ptr<AbstractBody>>&) const override;

        private:
            BodyAABBTree tree;
    };

}
