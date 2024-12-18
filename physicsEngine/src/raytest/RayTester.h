#pragma once

#include <UrchinCommon.h>

#include <raytest/RayTestResult.h>
#include <collision/CollisionWorld.h>

namespace urchin {

    class RayTester {
        public:
            RayTester(CollisionWorld&, const Ray<float>& ray);

            std::shared_ptr<const RayTestResult> getRayTestResult() const;

            void execute() const;

        private:
            CollisionWorld& collisionWorld;
            Ray<float> ray;

            std::shared_ptr<RayTestResult> rayTestResult;
    };

}
