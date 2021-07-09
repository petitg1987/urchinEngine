#pragma once

#include <UrchinCommon.h>

#include <PhysicsWorld.h>
#include <processable/Processable.h>
#include <processable/raytest/RayTestResult.h>
#include <collision/CollisionWorld.h>

namespace urchin {

    class RayTester : public Processable {
        public:
            RayTester(const PhysicsWorld*, Ray<float> ray);

            std::shared_ptr<const RayTestResult> getRayTestResult() const;

            void setup(float, const Vector3<float>&) override;
            void execute(float, const Vector3<float>&) override;

        private:
            CollisionWorld& collisionWorld;
            const Ray<float> ray;

            std::shared_ptr<RayTestResult> rayTestResult;
    };

}
