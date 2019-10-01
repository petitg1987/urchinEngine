#ifndef GREENCITY_BODYAABBTREE_H
#define GREENCITY_BODYAABBTREE_H

#include "body/work/AbstractWorkBody.h"
#include "collision/OverlappingPair.h"
#include "collision/broadphase/PairContainer.h"
#include "collision/broadphase/BroadPhaseAlgorithm.h"
#include "collision/broadphase/aabbtree/AABBTree.h"
#include "collision/broadphase/aabbtree/BodyAABBNodeData.h"

namespace urchin
{

    class BodyAABBTree : public AABBTree<AbstractWorkBody>
    {
        public:
            BodyAABBTree();
            ~BodyAABBTree() override;

            void addBody(AbstractWorkBody *, PairContainer *);
            void postAddObjectCallback(AABBNode<AbstractWorkBody> *) override;

            void removeBody(AbstractWorkBody *);
            void preRemoveObjectCallback(AABBNode<AbstractWorkBody> *) override;

            void updateBodies();
            void preUpdateObject(AABBNode<AbstractWorkBody> *) override;

            const std::vector<OverlappingPair *> &getOverlappingPairs() const;

        private:
            void computeOverlappingPairsFor(AABBNode<AbstractWorkBody> *);
            void createOverlappingPair(BodyAABBNodeData *, BodyAABBNodeData *);
            void removeOverlappingPairs(const BodyAABBNodeData *);

            void computeWorldBoundary();
            void controlBoundaries(AABBNode<AbstractWorkBody> *);

            PairContainer *defaultPairContainer;

            bool inInitializationPhase;
            float minYBoundary;
    };

}

#endif
