#ifndef URCHINENGINE_BODYAABBTREE_H
#define URCHINENGINE_BODYAABBTREE_H

#include "UrchinCommon.h"

#include "body/model/AbstractBody.h"
#include "collision/OverlappingPair.h"
#include "collision/broadphase/PairContainer.h"
#include "collision/broadphase/BroadPhaseAlgorithm.h"
#include "collision/broadphase/aabbtree/BodyAABBNodeData.h"

namespace urchin {

    class BodyAABBTree : public AABBTree<AbstractBody*> {
        public:
            BodyAABBTree();
            ~BodyAABBTree() override;

            void addBody(AbstractBody*, PairContainer*);
            void postAddObjectCallback(AABBNode<AbstractBody*>*) override;

            void removeBody(AbstractBody*);
            void preRemoveObjectCallback(AABBNode<AbstractBody*>*) override;

            void updateBodies();
            void preUpdateObjectCallback(AABBNode<AbstractBody*>*) override;

            const std::vector<OverlappingPair*>& getOverlappingPairs() const;

        private:
            void computeOverlappingPairsFor(AABBNode<AbstractBody*>*);
            void createOverlappingPair(BodyAABBNodeData*, BodyAABBNodeData*);
            void removeOverlappingPairs(const BodyAABBNodeData*);
            void removeAlternativePairContainerReferences(const AbstractBody*, PairContainer*);

            void computeWorldBoundary();
            void controlBoundaries(AABBNode<AbstractBody*>*) const;

            PairContainer* defaultPairContainer;

            bool inInitializationPhase;
            float minYBoundary;
    };

}

#endif
