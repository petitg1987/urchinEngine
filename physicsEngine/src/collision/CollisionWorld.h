#pragma once

#include <UrchinCommon.h>

#include <body/BodyManager.h>
#include <collision/ManifoldResult.h>
#include <collision/broadphase/BroadPhaseManager.h>
#include <collision/narrowphase/NarrowPhaseManager.h>
#include <collision/integration/IntegrateVelocity.h>
#include <collision/constraintsolver/ConstraintSolverManager.h>
#include <collision/island/IslandManager.h>
#include <collision/integration/IntegrateTransform.h>

namespace urchin {

    /**
    * Collision world: process integration, broad phase and narrow phase
    */
    class CollisionWorld : public Observable {
        public:
            explicit CollisionWorld(BodyManager&);
            ~CollisionWorld() override = default;

            enum NotificationType {
                COLLISION_RESULT_UPDATED
            };

            BroadPhaseManager& getBroadPhaseManager() const;
            NarrowPhaseManager& getNarrowPhaseManager() const;

            void process(float, const Vector3<float>&);

            const std::vector<ManifoldResult>& getLastUpdatedManifoldResults();

        private:
            BodyManager& bodyManager;

            std::unique_ptr<BroadPhaseManager> broadPhaseManager;
            std::unique_ptr<NarrowPhaseManager> narrowPhaseManager;
            std::unique_ptr<IntegrateVelocity> integrateVelocity;
            std::unique_ptr<ConstraintSolverManager> constraintSolverManager;
            std::unique_ptr<IslandManager> islandManager;
            std::unique_ptr<IntegrateTransform> integrateTransform;

            std::vector<ManifoldResult> manifoldResults;
    };

}
