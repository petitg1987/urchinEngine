#pragma once

#include <body/BodyManager.h>
#include <body/model/RigidBody.h>
#include <collision/broadphase/BroadPhase.h>
#include <collision/narrowphase/NarrowPhase.h>

namespace urchin {

    /**
    * Manager allowing to perform integration on bodies transformation
    */
    class IntegrateTransform {
        public:
            IntegrateTransform(const BodyManager&, const BroadPhase&, const NarrowPhase&);

            void process(float);

        private:
            void handleContinuousCollision(RigidBody&, const PhysicsTransform&, const PhysicsTransform&, float);

            static const float MAX_LINEAR_VELOCITY_FACTOR;

            const BodyManager& bodyManager;
            const BroadPhase& broadPhase;
            const NarrowPhase& narrowPhase;
    };

}
