#ifndef URCHINENGINE_COLLISIONWORLD_H
#define URCHINENGINE_COLLISIONWORLD_H

#include "UrchinCommon.h"

#include "body/BodyManager.h"
#include "collision/ManifoldResult.h"
#include "collision/broadphase/BroadPhaseManager.h"
#include "collision/narrowphase/NarrowPhaseManager.h"
#include "collision/integration/IntegrateVelocityManager.h"
#include "collision/constraintsolver/ConstraintSolverManager.h"
#include "collision/island/IslandManager.h"
#include "collision/integration/IntegrateTransformManager.h"

namespace urchin
{

    /**
    * Collision world: process integration, broad phase and narrow phase
    */
    class CollisionWorld : public Observable
    {
        public:
            explicit CollisionWorld(BodyManager *);
            ~CollisionWorld() override;

            enum NotificationType
            {
                COLLISION_RESULT_UPDATED
            };

            BroadPhaseManager *getBroadPhaseManager() const;
            NarrowPhaseManager *getNarrowPhaseManager() const;

            void process(float, const Vector3<float> &);

            const std::vector<ManifoldResult> &getLastUpdatedManifoldResults();

        private:
            BodyManager *bodyManager;

            BroadPhaseManager *broadPhaseManager;
            NarrowPhaseManager *narrowPhaseManager;
            IntegrateVelocityManager *integrateVelocityManager;
            ConstraintSolverManager *constraintSolverManager;
            IslandManager *islandManager;
            IntegrateTransformManager *integrateTransformManager;

            std::vector<ManifoldResult> manifoldResults;
    };

}

#endif
