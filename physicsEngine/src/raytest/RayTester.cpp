#include <collision/narrowphase/algorithm/continuous/ContinuousCollisionResult.h>
#include <raytest/RayTester.h>

namespace urchin {

    RayTester::RayTester(CollisionWorld& collisionWorld, const Ray<float>& ray) :
            collisionWorld(collisionWorld),
            ray(ray),
            rayTestVersion(1),
            rayTestResult(RayTestResult()) {

    }

    unsigned int RayTester::getRayTestVersion() const {
        return rayTestVersion;
    }

    void RayTester::updateRay(const Ray<float>& ray) {
        this->ray = ray;
        this->rayTestVersion++;
    }

    bool RayTester::isResultReady() const {
        return rayTestResult.isResultReady(rayTestVersion);
    }

    std::optional<ContinuousCollisionResult<float>> RayTester::getNearestResult() const {
        return rayTestResult.getNearestResult().second;
    }

    void RayTester::execute() {
        std::vector<std::shared_ptr<AbstractBody>> bodiesAABBoxHitRay = collisionWorld.getBroadPhase().rayTest(ray);
        ccd_set rayCastResults = collisionWorld.getNarrowPhase().rayTest(ray, bodiesAABBoxHitRay);

        rayTestResult.updateResults(rayCastResults, rayTestVersion);
    }

}
