#pragma once

#include <UrchinCommon.h>

#include "body/BodyContainer.h"
#include "collision/ManifoldResult.h"
#include "collision/broadphase/BroadPhase.h"
#include "collision/narrowphase/NarrowPhase.h"
#include "collision/integration/IntegrateVelocity.h"
#include "collision/constraintsolver/ConstraintSolver.h"
#include "collision/bodystate/BodyActiveStateUpdater.h"
#include "collision/integration/IntegrateTransform.h"

namespace urchin {

    /**
    * Collision world: process integration, broad phase and narrow phase
    */
    class CollisionWorld final : public Observable {
        public:
            explicit CollisionWorld(BodyContainer&);
            ~CollisionWorld() override = default;

            enum NotificationType {
                COLLISION_RESULT_UPDATED
            };

            BroadPhase& getBroadPhase();
            NarrowPhase& getNarrowPhase();

            void process(float, const Vector3<float>&);

            const std::vector<ManifoldResult>& getLastUpdatedManifoldResults() const;

        private:
            BodyContainer& bodyContainer;

            BroadPhase broadPhase;
            NarrowPhase narrowPhase;
            IntegrateVelocity integrateVelocity;
            ConstraintSolver constraintSolver;
            BodyActiveStateUpdater bodyActiveStateUpdater;
            IntegrateTransform integrateTransform;

            std::vector<ManifoldResult> manifoldResults;
    };

}
