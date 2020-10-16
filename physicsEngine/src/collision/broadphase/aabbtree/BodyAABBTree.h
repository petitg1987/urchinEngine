#ifndef URCHINENGINE_BODYAABBTREE_H
#define URCHINENGINE_BODYAABBTREE_H

#include "UrchinCommon.h"

#include "body/work/AbstractWorkBody.h"
#include "collision/OverlappingPair.h"
#include "collision/broadphase/PairContainer.h"
#include "collision/broadphase/BroadPhaseAlgorithm.h"
#include "collision/broadphase/aabbtree/BodyAABBNodeData.h"

namespace urchin {

    class BodyAABBTree : public AABBTree<AbstractWorkBody *> {
        public:
            BodyAABBTree();
            ~BodyAABBTree() override;

            void addBody(AbstractWorkBody *, PairContainer *);
            void postAddObjectCallback(AABBNode<AbstractWorkBody *> *) override;

            void removeBody(AbstractWorkBody *);
            void preRemoveObjectCallback(AABBNode<AbstractWorkBody *> *) override;

            void updateBodies();
            void preUpdateObjectCallback(AABBNode<AbstractWorkBody *> *) override;

            const std::vector<OverlappingPair *> &getOverlappingPairs() const;

        private:
            void computeOverlappingPairsfor (AABBNode<AbstractWorkBody *> *);
            void createOverlappingPair(BodyAABBNodeData *, BodyAABBNodeData *);
            void removeOverlappingPairs(const BodyAABBNodeData *);
            void removeAlternativePairContainerReferences(const AbstractWorkBody *, PairContainer *);

            void computeWorldBoundary();
            void controlBoundaries(AABBNode<AbstractWorkBody *> *);

            PairContainer *defaultPairContainer;

            bool inInitializationPhase;
            float minYBoundary;
    };

}

#endif
