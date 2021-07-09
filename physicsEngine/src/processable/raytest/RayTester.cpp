#include <collision/narrowphase/algorithm/continuous/result/ContinuousCollisionResult.h>
#include <processable/raytest/RayTester.h>

namespace urchin {

    RayTester::RayTester(const PhysicsWorld* physicsWorld, Ray<float> ray) :
            collisionWorld(physicsWorld->getCollisionWorld()),
            ray(ray),
            rayTestResult(std::make_shared<RayTestResult>()) {

    }

    std::shared_ptr<const RayTestResult> RayTester::getRayTestResult() const {
        return rayTestResult;
    }

    void RayTester::setup(float, const Vector3<float>&) {
        //nothing to do
    }

    void RayTester::execute(float, const Vector3<float>&) {
        std::vector<AbstractBody*> bodiesAABBoxHitRay = collisionWorld.getBroadPhaseManager()->rayTest(ray);
        ccd_set rayCastResults = collisionWorld.getNarrowPhaseManager()->rayTest(ray, bodiesAABBoxHitRay);

        rayTestResult->addResults(rayCastResults);
    }

}
