#pragma once

#include <UrchinCommon.h>

#include <body/BodyContainer.h>
#include <collision/ManifoldResult.h>
#include <collision/broadphase/BroadPhase.h>
#include <collision/narrowphase/NarrowPhase.h>
#include <collision/integration/IntegrateVelocity.h>
#include <collision/constraintsolver/ConstraintSolver.h>
#include <collision/bodystate/BodyActiveStateUpdater.h>
#include <collision/integration/IntegrateTransform.h>

namespace urchin {

    /**
    * Collision world: process integration, broad phase and narrow phase
    */
    class CollisionWorld : public Observable {
        public:
            explicit CollisionWorld(BodyContainer&);
            ~CollisionWorld() override = default;

            enum NotificationType {
                COLLISION_RESULT_UPDATED
            };

            BroadPhase& getBroadPhase() const;
            NarrowPhase& getNarrowPhase() const;

            void process(float, const Vector3<float>&);

            const std::vector<ManifoldResult>& getLastUpdatedManifoldResults();

        private:
            BodyContainer& bodyContainer;

            std::unique_ptr<BroadPhase> broadPhase;
            std::unique_ptr<NarrowPhase> narrowPhase;
            std::unique_ptr<IntegrateVelocity> integrateVelocity;
            std::unique_ptr<ConstraintSolver> constraintSolver;
            std::unique_ptr<BodyActiveStateUpdater> bodyActiveStateUpdater;
            std::unique_ptr<IntegrateTransform> integrateTransform;

            std::vector<ManifoldResult> manifoldResults;
    };

}
