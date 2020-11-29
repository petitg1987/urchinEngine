#ifndef URCHINENGINE_BROADPHASEMANAGER_H
#define URCHINENGINE_BROADPHASEMANAGER_H

#include <vector>
#include "UrchinCommon.h"

#include "collision/broadphase/BroadPhaseAlgorithm.h"
#include "collision/broadphase/PairContainer.h"
#include "collision/OverlappingPair.h"
#include "body/work/AbstractWorkBody.h"
#include "body/BodyManager.h"

namespace urchin {

    /**
    * Broad phase manager allowing to determine pairs of bodies potentially colliding
    */
    class BroadPhaseManager : public Observer {
        public:
            explicit BroadPhaseManager(BodyManager*);
            ~BroadPhaseManager() override;

            void notify(Observable*, int) override;

            void addBodyAsync(AbstractWorkBody*);
            void removeBodyAsync(AbstractWorkBody*);

            const std::vector<OverlappingPair *>& computeOverlappingPairs();

            std::vector<AbstractWorkBody *> rayTest(const Ray<float>&) const;
            std::vector<AbstractWorkBody *> bodyTest(AbstractWorkBody*, const PhysicsTransform&, const PhysicsTransform&) const;

        private:
            void addBody(AbstractWorkBody*);
            void removeBody(AbstractWorkBody*);
            void synchronizeBodies();

            BroadPhaseAlgorithm *broadPhaseAlgorithm;

            std::mutex mutex;
            std::vector<AbstractWorkBody *> bodiesToAdd;
            std::vector<AbstractWorkBody *> bodiesToRemove;
    };

}

#endif
