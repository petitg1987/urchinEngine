#pragma once

#include <vector>
#include <UrchinCommon.h>

#include <collision/broadphase/BroadPhaseAlgorithm.h>
#include <collision/broadphase/PairContainer.h>
#include <collision/OverlappingPair.h>
#include <body/model/AbstractBody.h>
#include <body/BodyManager.h>

namespace urchin {

    /**
    * Broad phase manager allowing to determine pairs of bodies potentially colliding
    */
    class BroadPhaseManager : public Observer {
        public:
            explicit BroadPhaseManager(BodyManager&);
            ~BroadPhaseManager() override;

            void notify(Observable*, int) override;

            void addBodyAsync(AbstractBody*);
            void removeBodyAsync(AbstractBody*);

            const std::vector<std::unique_ptr<OverlappingPair>>& computeOverlappingPairs();

            std::vector<AbstractBody*> rayTest(const Ray<float>&) const;
            std::vector<AbstractBody*> bodyTest(AbstractBody*, const PhysicsTransform&, const PhysicsTransform&) const;

        private:
            void addBody(AbstractBody*);
            void removeBody(AbstractBody*);
            void synchronizeBodies();

            std::unique_ptr<BroadPhaseAlgorithm> broadPhaseAlgorithm;

            std::mutex mutex;
            std::vector<AbstractBody*> bodiesToAdd;
            std::vector<AbstractBody*> bodiesToRemove;
    };

}
