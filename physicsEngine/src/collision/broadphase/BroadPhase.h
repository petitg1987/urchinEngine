#pragma once

#include <vector>
#include <UrchinCommon.h>

#include <collision/broadphase/BroadPhaseAlgorithm.h>
#include <collision/broadphase/PairContainer.h>
#include <collision/OverlappingPair.h>
#include <body/model/AbstractBody.h>
#include <body/BodyContainer.h>

namespace urchin {

    /**
    * Broad phase manager allowing to determine pairs of bodies potentially colliding
    */
    class BroadPhase : public Observer {
        public:
            explicit BroadPhase(BodyContainer&);
            ~BroadPhase() override;

            void notify(Observable*, int) override;

            void addBodyAsync(const std::shared_ptr<AbstractBody>&);
            void removeBodyAsync(const std::shared_ptr<AbstractBody>&);

            const std::vector<std::unique_ptr<OverlappingPair>>& computeOverlappingPairs();

            std::vector<std::shared_ptr<AbstractBody>> rayTest(const Ray<float>&) const;
            std::vector<std::shared_ptr<AbstractBody>> bodyTest(const AbstractBody&, const PhysicsTransform&, const PhysicsTransform&) const;

        private:
            void addBody(std::shared_ptr<AbstractBody>);
            void removeBody(const AbstractBody&);
            void synchronizeBodies();

            std::unique_ptr<BroadPhaseAlgorithm> broadPhaseAlgorithm;

            std::mutex mutex;
            std::vector<std::shared_ptr<AbstractBody>> bodiesToAdd;
            std::vector<std::shared_ptr<const AbstractBody>> bodiesToRemove;
    };

}
