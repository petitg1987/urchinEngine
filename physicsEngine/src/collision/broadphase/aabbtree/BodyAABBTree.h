#pragma once

#include <UrchinCommon.h>

#include <body/model/AbstractBody.h>
#include <collision/OverlappingPair.h>
#include <collision/broadphase/PairContainer.h>
#include <collision/broadphase/BroadPhaseAlgorithm.h>
#include <collision/broadphase/aabbtree/BodyAABBNodeData.h>

namespace urchin {

    class BodyAABBTree : public AABBTree<std::shared_ptr<AbstractBody>> {
        public:
            BodyAABBTree();
            ~BodyAABBTree() override = default;

            void addBody(const std::shared_ptr<AbstractBody>&);
            void postAddObjectCallback(AABBNode<std::shared_ptr<AbstractBody>>&) override;

            void removeBody(const AbstractBody&);
            void preRemoveObjectCallback(AABBNode<std::shared_ptr<AbstractBody>>&) override;

            void updateBodies();
            void preUpdateObjectCallback(AABBNode<std::shared_ptr<AbstractBody>>&) override;

            const std::vector<std::unique_ptr<OverlappingPair>>& getOverlappingPairs() const;

        private:
            void computeOverlappingPairsFor(AABBNode<std::shared_ptr<AbstractBody>>&);
            void createOverlappingPair(BodyAABBNodeData&, BodyAABBNodeData&);
            void removeOverlappingPairs(const BodyAABBNodeData&);
            void removeBodyPairContainerReferences(const AbstractBody&, PairContainer*);

            void computeWorldBoundary();
            void controlBoundaries(AABBNode<std::shared_ptr<AbstractBody>>&) const;

            static constexpr float BOUNDARIES_MARGIN_PERCENTAGE = 0.3f;

            std::unique_ptr<PairContainer> defaultPairContainer;

            bool inInitializationPhase;
            float minYBoundary;
    };

}
