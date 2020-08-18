#ifndef URCHINENGINE_RAYTESTER_H
#define URCHINENGINE_RAYTESTER_H

#include "UrchinCommon.h"

#include "PhysicsWorld.h"
#include "processable/Processable.h"
#include "processable/raytest/RayTestResult.h"
#include "collision/CollisionWorld.h"

namespace urchin
{

    class RayTester : public Processable
    {
        public:
            explicit RayTester(Ray<float> ray);

            std::shared_ptr<const RayTestResult> getRayTestResult() const;

            void initialize(PhysicsWorld *) override;

            void setup(float, const Vector3<float> &) override;
            void execute(float, const Vector3<float> &) override;

        private:
            const Ray<float> ray;
            std::shared_ptr<RayTestResult> rayTestResult;

            CollisionWorld *collisionWorld;

    };

}

#endif
