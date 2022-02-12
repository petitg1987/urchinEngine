#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <memory>
#include <cstdio>

#include <physics/collision/CollisionWorldIT.h>
#include <AssertHelper.h>
using namespace urchin;

void CollisionWorldIT::fallOnGround() {
    auto bodyContainer = buildWorld(Point3<float>(0.0f, 5.0f, 0.0f));
    auto collisionWorld = std::make_unique<CollisionWorld>(*bodyContainer);

    for (std::size_t i = 0; i < 250; ++i) {
        collisionWorld->process(1.0f / 60.0f, Vector3<float>(0.0f, -9.81f, 0.0f));
    }

    auto cubeBody = bodyContainer->getBodies()[1];
    AssertHelper::assertFloatEquals(cubeBody->getTransform().getPosition().Y, 0.5f, 0.1f);
    AssertHelper::assertTrue(!cubeBody->isActive(), "Body must become inactive when it doesn't move");
}

void CollisionWorldIT::ccdPushOnGround() {
    auto bodyContainer = buildWorld(Point3<float>(0.0f, 5.0f, 0.0f));
    auto collisionWorld = std::make_unique<CollisionWorld>(*bodyContainer);
    collisionWorld->process(1.0f / 1000.0f, Vector3<float>(0.0f, 0.0f, 0.0f));
    auto* cubeBody = static_cast<RigidBody*>(bodyContainer->getBodies()[1].get());
    cubeBody->applyCentralMomentum(Vector3<float>(0.0f, -1000.0f, 0.0f)); //apply extreme down force
    cubeBody->setLinearFactor(Vector3<float>(0.0f, 1.0f, 0.0f)); //avoid cube to go on outside world border
    cubeBody->setAngularFactor(Vector3<float>(0.0f, 0.0f, 0.0f)); //avoid cube to go on outside world border

    for (std::size_t i = 0; i < 500; ++i) {
        collisionWorld->process(1.0f / 60.0f, Vector3<float>(0.0f, -9.81f, 0.0f));
        AssertHelper::assertTrue(cubeBody->getTransform().getPosition().Y > 0.4f); //ensure narrow phase predictive contact point works correctly to avoid penetration
    }

    AssertHelper::assertFloatEquals(cubeBody->getTransform().getPosition().Y, 0.5f, 0.1f);
    AssertHelper::assertTrue(!cubeBody->isActive(), "Body must become inactive when it doesn't move");
}

void CollisionWorldIT::ccdBounceOnGroundAndRoof() {
    auto bodyContainer = buildWorld(Point3<float>(0.0f, 5.0f, 0.0f));
    auto roofShape = std::make_unique<CollisionBoxShape>(Vector3<float>(50.0f, 0.5f, 50.0f));
    bodyContainer->addBody(std::make_unique<RigidBody>("roof", PhysicsTransform(Point3<float>(0.0f, 10.0f, 0.0f), Quaternion<float>()), std::move(roofShape)));
    auto collisionWorld = std::make_unique<CollisionWorld>(*bodyContainer);
    collisionWorld->process(1.0f / 1000.0f, Vector3<float>(0.0f, 0.0f, 0.0f));
    auto* cubeBody = static_cast<RigidBody*>(bodyContainer->getBodies()[1].get());
    cubeBody->applyCentralMomentum(Vector3<float>(0.0f, -10000.0f, 0.0f)); //apply extreme down force
    cubeBody->setRestitution(0.9f); //ensure cube bouncing well

    //Execute physics process:
    //  - The constraint solver computes a linear velocity in Y of +700.0 (based on predictive contact point) as an answer to the big collision on the ground.
    //  - The integrate transform should be smart enough to not apply directly the linear velocity and make the cube going through roof. Instead, it should adjust/reduce the linear velocity before apply it.
    collisionWorld->process(1.0f / 30.0f, Vector3<float>(0.0f, -9.81f, 0.0f));

    AssertHelper::assertTrue(cubeBody->getTransform().getPosition().Y < 10.0f);
}

void CollisionWorldIT::fallForever() {
    Logger::instance().purge(); //Log file must be emptied before start this test
    auto bodyContainer = buildWorld(Point3<float>(60.0f, 5.0f, 0.0f));
    auto collisionWorld = std::make_unique<CollisionWorld>(*bodyContainer);

    for (std::size_t i = 0; i < 250; ++i) {
        collisionWorld->process(1.0f / 60.0f, Vector3<float>(0.0f, -9.81f, 0.0f));
    }

    auto cubeBody = bodyContainer->getBodies()[1];
    AssertHelper::assertTrue(cubeBody->getTransform().getPosition().Y > -4.0f, "Check cube doesn't fall forever");
    AssertHelper::assertTrue(!cubeBody->isActive(), "Body must become inactive when it goes outside the world limits");
    std::string logValue = Logger::instance().retrieveContent(std::numeric_limits<unsigned long>::max());
    AssertHelper::assertTrue(logValue.find("(WW) Body cube is below the limit of") != std::string::npos);
    Logger::instance().purge();
}

void CollisionWorldIT::changePositionOnInactiveBody() {
    auto bodyContainer = buildWorld(Point3<float>(0.0f, 0.5f, 0.0f));
    auto collisionWorld = std::make_unique<CollisionWorld>(*bodyContainer);

    //1. make sure body is inactive on the ground
    for (std::size_t i = 0; i < 25; ++i) {
        collisionWorld->process(1.0f / 60.0f, Vector3<float>(0.0f, -9.81f, 0.0f));
    }
    auto cubeBody = bodyContainer->getBodies()[1];
    AssertHelper::assertFloatEquals(cubeBody->getTransform().getPosition().Y, 0.5f, 0.1f);
    AssertHelper::assertTrue(!cubeBody->isActive(), "Body must become inactive when it doesn't move (1)");

    //2. change position (in thread different from physics thread)
    auto thread = std::jthread([&cubeBody]() {
        cubeBody->setTransform(PhysicsTransform(Point3<float>(25.0, 5.0, 0.0), Quaternion<float>()));
    });
    thread.join();
    for (std::size_t i = 0; i < 250; ++i) {
        collisionWorld->process(1.0f / 60.0f, Vector3<float>(0.0f, -9.81f, 0.0f));
    }
    AssertHelper::assertFloatEquals(cubeBody->getTransform().getPosition().X, 25.0f, 2.5f);
    AssertHelper::assertFloatEquals(cubeBody->getTransform().getPosition().Y, 0.5f, 0.1f);
    AssertHelper::assertTrue(!cubeBody->isActive(), "Body must become inactive when it doesn't move (2)");
}

void CollisionWorldIT::changeMomentumOnInactiveBody() {
    auto bodyContainer = buildWorld(Point3<float>(0.0f, 0.5f, 0.0f));
    auto collisionWorld = std::make_unique<CollisionWorld>(*bodyContainer);

    //1. make sure body is inactive on the ground
    for (std::size_t i = 0; i < 25; ++i) {
        collisionWorld->process(1.0f / 60.0f, Vector3<float>(0.0f, -9.81f, 0.0f));
    }
    auto* cubeBody = static_cast<RigidBody*>(bodyContainer->getBodies()[1].get());
    AssertHelper::assertFloatEquals(cubeBody->getTransform().getPosition().Y, 0.5f, 0.1f);
    AssertHelper::assertTrue(!cubeBody->isActive(), "Body must become inactive when it doesn't move (1)");

    //2. apply momentum
    cubeBody->applyCentralMomentum(Vector3<float>(100.0, 100.0, 0.0));
    for (std::size_t i = 0; i < 250; ++i) {
        collisionWorld->process(1.0f / 60.0f, Vector3<float>(0.0f, -9.81f, 0.0f));
    }
    AssertHelper::assertFloatEquals(cubeBody->getTransform().getPosition().X, 25.0f, 2.5f);
    AssertHelper::assertFloatEquals(cubeBody->getTransform().getPosition().Y, 0.5f, 0.1f);
    AssertHelper::assertTrue(!cubeBody->isActive(), "Body must become inactive when it doesn't move (2)");
}

void CollisionWorldIT::changeMass() {
    auto bodyContainer = buildWorld(Point3<float>(0.0f, 10.0f, 0.0f));
    auto collisionWorld = std::make_unique<CollisionWorld>(*bodyContainer);

    //1. make sure cube fall
    for (std::size_t i = 0; i < 5; ++i) {
        collisionWorld->process(1.0f / 60.0f, Vector3<float>(0.0f, -9.81f, 0.0f));
    }
    auto* cubeBody = static_cast<RigidBody*>(bodyContainer->getBodies()[1].get());
    AssertHelper::assertFloatEquals(cubeBody->getTransform().getPosition().Y, 9.96f, 0.01f);

    //2. change cube mass to 0.0f (static body)
    cubeBody->setMass(0.0f);
    for (std::size_t i = 0; i < 250; ++i) {
        collisionWorld->process(1.0f / 60.0f, Vector3<float>(0.0f, -9.81f, 0.0f));
    }
    AssertHelper::assertFloatEquals(cubeBody->getTransform().getPosition().Y, 9.96f, 0.01f);

    //3. change cube mass to 10.0f (dynamic body)
    cubeBody->setMass(10.0f);
    for (std::size_t i = 0; i < 250; ++i) {
        collisionWorld->process(1.0f / 60.0f, Vector3<float>(0.0f, -9.81f, 0.0f));
    }
    AssertHelper::assertFloatEquals(cubeBody->getTransform().getPosition().Y, 0.5f, 0.1f);
    AssertHelper::assertTrue(!cubeBody->isActive(), "Body must become inactive when it doesn't move");
}

void CollisionWorldIT::rayTestWithRemovedBody() {
    auto bodyContainer = buildWorld(Point3<float>(0.0f, 0.5f, 0.0f));
    auto collisionWorld = std::make_unique<CollisionWorld>(*bodyContainer);
    RayTester rayTester(*collisionWorld, Ray(Point3<float>(0.0f, 0.5f, -100.0f), Point3<float>(0.0f, 0.5f, 100.0f)));

    auto physicsThread = std::jthread([&]() {
        collisionWorld->process(1.0f / 60.0f, Vector3<float>(0.0f, -9.81f, 0.0f));
        rayTester.execute(0.0f, Vector3<float>());

        std::weak_ptr<AbstractBody> cubeBody = bodyContainer->getBodies()[1];
        bodyContainer->removeBody(*cubeBody.lock());
        collisionWorld->process(1.0f / 60.0f, Vector3<float>(0.0f, -9.81f, 0.0f));
        AssertHelper::assertUnsignedIntEquals((unsigned int)cubeBody.use_count(), 1 /* one reference to the body remaining in the ray test result */);
    });
    physicsThread.join();

    std::shared_ptr<const RayTestResult> rayTestResult = rayTester.getRayTestResult();
    AssertHelper::assertTrue(rayTestResult->isResultReady());
    AssertHelper::assertTrue(rayTestResult->hasHit());
    AssertHelper::assertStringEquals(rayTestResult->getNearestResult().getBody2().getId(), "cube");
    AssertHelper::assertVector3FloatEquals(rayTestResult->getNearestResult().getNormalFromObject2(), Vector3<float>(0.0f, 0.0f, -1.0f), 0.01f);
    AssertHelper::assertPoint3FloatEquals(rayTestResult->getNearestResult().getHitPointOnObject2(), Point3<float>(0.0f, 0.5f, -0.5f), 0.01f);
}

std::unique_ptr<BodyContainer> CollisionWorldIT::buildWorld(const Point3<float>& cubePosition) const {
    auto bodyContainer = std::make_unique<BodyContainer>();

    auto groundShape = std::make_unique<CollisionBoxShape>(Vector3<float>(50.0f, 0.5f, 50.0f));
    auto groundBody = std::make_unique<RigidBody>("ground", PhysicsTransform(Point3<float>(0.0f, -0.5f, 0.0f), Quaternion<float>()), std::move(groundShape));
    bodyContainer->addBody(std::move(groundBody));

    auto cubeShape = std::make_unique<CollisionBoxShape>(Vector3<float>(0.5f, 0.5f, 0.5f));
    auto cubeBody = std::make_unique<RigidBody>("cube", PhysicsTransform(cubePosition, Quaternion<float>()), std::move(cubeShape));
    cubeBody->setMass(10.0f);
    bodyContainer->addBody(std::move(cubeBody));

    return bodyContainer;
}

CppUnit::Test* CollisionWorldIT::suite() {
    auto* suite = new CppUnit::TestSuite("CollisionWorldIT");

    suite->addTest(new CppUnit::TestCaller<CollisionWorldIT>("fallOnGround", &CollisionWorldIT::fallOnGround));
    suite->addTest(new CppUnit::TestCaller<CollisionWorldIT>("ccdPushOnGround", &CollisionWorldIT::ccdPushOnGround));
    suite->addTest(new CppUnit::TestCaller<CollisionWorldIT>("ccdBounceOnGroundAndRoof", &CollisionWorldIT::ccdBounceOnGroundAndRoof));
    suite->addTest(new CppUnit::TestCaller<CollisionWorldIT>("fallForever", &CollisionWorldIT::fallForever));

    suite->addTest(new CppUnit::TestCaller<CollisionWorldIT>("changePositionOnInactiveBody", &CollisionWorldIT::changePositionOnInactiveBody));
    suite->addTest(new CppUnit::TestCaller<CollisionWorldIT>("changeMomentumOnInactiveBody", &CollisionWorldIT::changeMomentumOnInactiveBody));

    suite->addTest(new CppUnit::TestCaller<CollisionWorldIT>("changeMass", &CollisionWorldIT::changeMass));

    suite->addTest(new CppUnit::TestCaller<CollisionWorldIT>("rayTestWithRemovedBody", &CollisionWorldIT::rayTestWithRemovedBody));

    return suite;
}
