#pragma once

#include <UrchinCommon.h>

#include <body/model/AbstractBody.h>
#include <collision/OverlappingPair.h>
#include <collision/broadphase/BroadPhaseAlgorithm.h>
#include <collision/broadphase/aabbtree/BodyAABBTree.h>

namespace urchin {

    class AABBTreeAlgorithm : public BroadPhaseAlgorithm {
        public:
            AABBTreeAlgorithm();

            void addBody(AbstractBody&) override;
            void removeBody(const AbstractBody&) override;
            void updateBodies() override;

            const std::vector<std::unique_ptr<OverlappingPair>>& getOverlappingPairs() const override;

            std::vector<AbstractBody*> rayTest(const Ray<float>&) const override;
            std::vector<AbstractBody*> bodyTest(const AbstractBody&, const PhysicsTransform&, const PhysicsTransform&) const override;

        private:
            std::unique_ptr<BodyAABBTree> tree;

    };

}
