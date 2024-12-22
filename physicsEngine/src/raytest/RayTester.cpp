#include <collision/narrowphase/algorithm/continuous/ContinuousCollisionResult.h>
#include <raytest/RayTester.h>

namespace urchin {

    RayTester::RayTester() :
            ray(Ray(Point3(0.0f, 0.0f, 0.0f), Point3(0.0f, 0.0f, 0.0f))),
            rayTestVersion(0),
            rayTestResult(RayTestResult()) {

    }

    std::shared_ptr<RayTester> RayTester::newRayTester() {
        return std::make_shared<RayTester>();
    }

    void RayTester::updateRay(const Ray<float>& ray) {
        this->ray = ray;
        this->rayTestVersion++;
    }

    bool RayTester::isRayTestTriggerred() const {
        return rayTestVersion != 0;
    }

    bool RayTester::isResultReady() const {
        return isRayTestTriggerred() && rayTestResult.isResultReady(rayTestVersion);
    }

    std::optional<ContinuousCollisionResult<float>> RayTester::getNearestResult() const {
        return rayTestResult.getNearestResult().second;
    }

    void RayTester::execute(CollisionWorld& collisionWorld) {
        collisionWorld.getBroadPhase().rayTest(ray, bodiesAABBoxHitRay);

        collisionWorld.getNarrowPhase().rayTest(ray, bodiesAABBoxHitRay, rayCastResults);
        bodiesAABBoxHitRay.clear();

        rayTestResult.updateResults(rayCastResults, rayTestVersion);
        rayCastResults.clear();
    }

}
