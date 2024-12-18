#include <collision/narrowphase/algorithm/continuous/ContinuousCollisionResult.h>
#include <raytest/RayTester.h>

namespace urchin {

    RayTester::RayTester(CollisionWorld& collisionWorld, const Ray<float>& ray) :
            collisionWorld(collisionWorld),
            ray(ray),
            rayTestResult(std::make_shared<RayTestResult>()) {

    }

    std::shared_ptr<const RayTestResult> RayTester::getRayTestResult() const {
        return rayTestResult;
    }

    void RayTester::execute() const {
        std::vector<std::shared_ptr<AbstractBody>> bodiesAABBoxHitRay = collisionWorld.getBroadPhase().rayTest(ray);
        ccd_set rayCastResults = collisionWorld.getNarrowPhase().rayTest(ray, bodiesAABBoxHitRay);

        rayTestResult->addResults(rayCastResults);
    }

}
