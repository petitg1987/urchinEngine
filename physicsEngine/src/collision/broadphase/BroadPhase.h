#pragma once

#include <vector>
#include <UrchinCommon.h>

#include <collision/broadphase/aabbtree/AABBTreeAlgorithm.h>
#include <collision/OverlappingPair.h>
#include <body/model/AbstractBody.h>
#include <body/BodyContainer.h>

namespace urchin {

    /**
    * Broad phase manager allowing to determine pairs of bodies potentially colliding
    */
    class BroadPhase final : public Observer {
        public:
            explicit BroadPhase(BodyContainer&);
            ~BroadPhase() override;

            void notify(Observable*, int) override;

            void addBodyAsync(const std::shared_ptr<AbstractBody>&);
            void removeBodyAsync(const std::shared_ptr<AbstractBody>&);

            const std::vector<std::unique_ptr<OverlappingPair>>& computeOverlappingPairs();

            void rayTest(const Ray<float>&, std::vector<std::shared_ptr<AbstractBody>>&) const;
            void bodyTest(const AbstractBody&, const PhysicsTransform&, const PhysicsTransform&, std::vector<std::shared_ptr<AbstractBody>>&) const;

        private:
            void addBody(const std::shared_ptr<AbstractBody>&);
            void removeBody(const AbstractBody&);
            void synchronizeBodies();

            AABBTreeAlgorithm broadPhaseAlgorithm;

            std::mutex mutex;
            std::vector<std::shared_ptr<AbstractBody>> bodiesToAdd;
            std::vector<std::shared_ptr<const AbstractBody>> bodiesToRemove;
    };

}
