#ifndef URCHINENGINE_AABBTREEALGORITHM_H
#define URCHINENGINE_AABBTREEALGORITHM_H

#include "UrchinCommon.h"

#include "body/model/AbstractBody.h"
#include "collision/OverlappingPair.h"
#include "collision/broadphase/BroadPhaseAlgorithm.h"
#include "collision/broadphase/aabbtree/BodyAABBTree.h"

namespace urchin {

    class AABBTreeAlgorithm : public BroadPhaseAlgorithm {
        public:
            AABBTreeAlgorithm();
            ~AABBTreeAlgorithm() override;

            void addBody(AbstractBody*, PairContainer*) override;
            void removeBody(AbstractBody*) override;
            void updateBodies() override;

            const std::vector<OverlappingPair*>& getOverlappingPairs() const override;

            std::vector<AbstractBody*> rayTest(const Ray<float>&) const override;
            std::vector<AbstractBody*> bodyTest(AbstractBody*, const PhysicsTransform&, const PhysicsTransform&) const override;

        private:
            BodyAABBTree* tree;

    };

}

#endif
