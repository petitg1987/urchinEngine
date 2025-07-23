#pragma once

#include "body/BodyContainer.h"
#include "body/model/RigidBody.h"
#include "collision/broadphase/BroadPhase.h"
#include "collision/narrowphase/NarrowPhase.h"

namespace urchin {

    /**
    * Manager allowing to perform integration on bodies transformation
    */
    class IntegrateTransform {
        public:
            IntegrateTransform(const BodyContainer&, const BroadPhase&, const NarrowPhase&);

            void process(float) const;

        private:
            void handleContinuousCollision(RigidBody&, const PhysicsTransform&, const PhysicsTransform&, float) const;

            static constexpr float MAX_LINEAR_VELOCITY_FACTOR = 0.95f;

            const BodyContainer& bodyContainer;
            const BroadPhase& broadPhase;
            const NarrowPhase& narrowPhase;
    };

}
