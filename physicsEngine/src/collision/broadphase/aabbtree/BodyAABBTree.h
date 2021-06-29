#pragma once

#include <UrchinCommon.h>

#include <body/model/AbstractBody.h>
#include <collision/OverlappingPair.h>
#include <collision/broadphase/PairContainer.h>
#include <collision/broadphase/BroadPhaseAlgorithm.h>
#include <collision/broadphase/aabbtree/BodyAABBNodeData.h>

namespace urchin {

    class BodyAABBTree : public AABBTree<AbstractBody*> {
        public:
            BodyAABBTree();
            ~BodyAABBTree() override;

            void addBody(AbstractBody*);
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
            void removeBodyPairContainerReferences(const AbstractBody*, PairContainer*);

            void computeWorldBoundary();
            void controlBoundaries(AABBNode<AbstractBody*>*) const;

            static const float BOUNDARIES_MARGIN_PERCENTAGE;

            PairContainer* defaultPairContainer;

            bool inInitializationPhase;
            float minYBoundary;
    };

}
