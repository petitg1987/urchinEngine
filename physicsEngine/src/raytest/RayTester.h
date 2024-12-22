#pragma once

#include <UrchinCommon.h>

#include <raytest/RayTestResult.h>
#include <collision/CollisionWorld.h>

class CollisionWorldIT;

namespace urchin {

    class RayTester {
        public:
            friend class PhysicsWorld;
            friend class ::CollisionWorldIT;

            RayTester(CollisionWorld&, const Ray<float>&);

            unsigned int getRayTestVersion() const;

            bool isResultReady() const;
            std::optional<ContinuousCollisionResult<float>> getNearestResult() const;

        protected:
            void updateRay(const Ray<float>&);

            void execute();

        private:

            CollisionWorld& collisionWorld;
            Ray<float> ray;
            unsigned int rayTestVersion;

            RayTestResult rayTestResult;
    };

}
