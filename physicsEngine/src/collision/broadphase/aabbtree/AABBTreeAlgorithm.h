#ifndef URCHINENGINE_AABBTREEALGORITHM_H
#define URCHINENGINE_AABBTREEALGORITHM_H

#include "UrchinCommon.h"

#include "body/work/AbstractWorkBody.h"
#include "collision/OverlappingPair.h"
#include "collision/broadphase/BroadPhaseAlgorithm.h"
#include "collision/broadphase/aabbtree/BodyAABBTree.h"

namespace urchin {

    class AABBTreeAlgorithm : public BroadPhaseAlgorithm {
        public:
            AABBTreeAlgorithm();
            ~AABBTreeAlgorithm() override;

            void addBody(AbstractWorkBody*, PairContainer*) override;
            void removeBody(AbstractWorkBody*) override;
            void updateBodies() override;

            const std::vector<OverlappingPair *>& getOverlappingPairs() const override;

            std::vector<AbstractWorkBody *> rayTest(const Ray<float>&) const override;
            std::vector<AbstractWorkBody *> bodyTest(AbstractWorkBody*, const PhysicsTransform&, const PhysicsTransform&) const override;

        private:
            BodyAABBTree *tree;

    };

}

#endif
