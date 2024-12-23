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

            RayTester();
            static std::shared_ptr<RayTester> newRayTester();

            bool isRayTestTriggerred() const;
            bool isResultReady() const;
            std::optional<ContinuousCollisionResult<float>> getNearestResult() const;

        protected:
            void updateRay(const Ray<float>&);

            void execute(CollisionWorld&);

        private:
            Ray<float> ray;
            unsigned int rayTestVersion;

            std::vector<std::shared_ptr<AbstractBody>> bodiesAABBoxHitRay;
            ccd_container rayCastResults;
            RayTestResult rayTestResult;
    };

}
