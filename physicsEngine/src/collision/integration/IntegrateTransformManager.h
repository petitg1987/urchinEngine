#pragma once

#include <body/BodyManager.h>
#include <body/model/RigidBody.h>
#include <collision/broadphase/BroadPhaseManager.h>
#include <collision/narrowphase/NarrowPhaseManager.h>

namespace urchin {

    /**
    * Manager allowing to perform integration on bodies transformation
    */
    class IntegrateTransformManager {
        public:
            IntegrateTransformManager(const BodyManager&, const BroadPhaseManager&, const NarrowPhaseManager&);

            void integrateTransform(float);

        private:
            void handleContinuousCollision(RigidBody&, const PhysicsTransform&, const PhysicsTransform&, float);

            static const float MAX_LINEAR_VELOCITY_FACTOR;

            const BodyManager& bodyManager;
            const BroadPhaseManager& broadPhaseManager;
            const NarrowPhaseManager& narrowPhaseManager;
    };

}
