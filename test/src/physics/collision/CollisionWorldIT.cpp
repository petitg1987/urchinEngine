#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <memory>
#include <cstdio>

#include "physics/collision/CollisionWorldIT.h"
#include "AssertHelper.h"
using namespace urchin;

void CollisionWorldIT::fallOnPlane() {
    auto bodyManager = buildWorld(Point3<float>(0.0f, 5.0f, 0.0f));
    auto collisionWorld = std::make_unique<CollisionWorld>(bodyManager.get());

    for (std::size_t i = 0; i < 250; ++i) {
        collisionWorld->process(1.0f / 60.0f, Vector3<float>(0.0f, -9.81f, 0.0f));
    }

    auto* cubeBody = bodyManager->getBodies()[1];
    AssertHelper::assertFloatEquals(cubeBody->getTransform().getPosition().Y, 0.5f, 0.1f);
    AssertHelper::assertTrue(!cubeBody->isActive(), "Body must become inactive when it doesn't move");
}

void CollisionWorldIT::fallForever() {
    if (!Logger::instance()->retrieveContent(std::numeric_limits<unsigned long>::max()).empty()) {
        throw std::runtime_error("Log file must be emptied before start this test.");
    }
    auto bodyManager = buildWorld(Point3<float>(1100.0f, 5.0f, 0.0f));
    auto collisionWorld = std::make_unique<CollisionWorld>(bodyManager.get());

    for (std::size_t i = 0; i < 250; ++i) {
        collisionWorld->process(1.0f / 60.0f, Vector3<float>(0.0f, -9.81f, 0.0f));
    }

    auto* cubeBody = bodyManager->getBodies()[1];
    AssertHelper::assertTrue(cubeBody->getTransform().getPosition().Y > -4.0f, "Check cube doesn't fall forever");
    AssertHelper::assertTrue(!cubeBody->isActive(), "Body must become inactive when it goes outside the world limits");
    std::string logValue = Logger::instance()->retrieveContent(std::numeric_limits<unsigned long>::max());
    AssertHelper::assertTrue(logValue.find("(WW) Body cube is below the limit of") != std::string::npos);
    Logger::instance()->purge();
}

void CollisionWorldIT::changePositionOnInactiveBody() {
    auto bodyManager = buildWorld(Point3<float>(0.0f, 0.5f, 0.0f));
    auto collisionWorld = std::make_unique<CollisionWorld>(bodyManager.get());

    //1. make sure body is inactive on the ground
    for (std::size_t i = 0; i < 25; ++i) {
        collisionWorld->process(1.0f / 60.0f, Vector3<float>(0.0f, -9.81f, 0.0f));
    }
    auto* cubeBody = bodyManager->getBodies()[1];
    AssertHelper::assertFloatEquals(cubeBody->getTransform().getPosition().Y, 0.5f, 0.1f);
    AssertHelper::assertTrue(!cubeBody->isActive(), "Body must become inactive when it doesn't move (1)");

    //2. change position (in thread different from physics thread)
    std::thread thread = std::thread([&cubeBody]() {
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
    auto bodyManager = buildWorld(Point3<float>(0.0f, 0.5f, 0.0f));
    auto collisionWorld = std::make_unique<CollisionWorld>(bodyManager.get());

    //1. make sure body is inactive on the ground
    for (std::size_t i = 0; i < 25; ++i) {
        collisionWorld->process(1.0f / 60.0f, Vector3<float>(0.0f, -9.81f, 0.0f));
    }
    auto* cubeBody = dynamic_cast<RigidBody*>(bodyManager->getBodies()[1]);
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

std::unique_ptr<BodyManager> CollisionWorldIT::buildWorld(const Point3<float>& cubePosition) const {
    auto bodyManager = std::make_unique<BodyManager>();

    std::shared_ptr<CollisionBoxShape> groundShape = std::make_shared<CollisionBoxShape>(Vector3<float>(1000.0f, 0.5f, 1000.0f));
    auto* groundBody = new RigidBody("ground", PhysicsTransform(Point3<float>(0.0f, -0.5f, 0.0f), Quaternion<float>()), groundShape);
    bodyManager->addBody(groundBody);

    std::shared_ptr<CollisionBoxShape> cubeShape = std::make_shared<CollisionBoxShape>(Vector3<float>(0.5f, 0.5f, 0.5f));
    auto* cubeBody = new RigidBody("cube", PhysicsTransform(cubePosition, Quaternion<float>()), cubeShape);
    cubeBody->setMass(10.0f);
    bodyManager->addBody(cubeBody);

    return bodyManager;
}

CppUnit::Test* CollisionWorldIT::suite() {
    auto* suite = new CppUnit::TestSuite("CollisionWorldIT");

    suite->addTest(new CppUnit::TestCaller<CollisionWorldIT>("fallOnPlane", &CollisionWorldIT::fallOnPlane));
    suite->addTest(new CppUnit::TestCaller<CollisionWorldIT>("fallForever", &CollisionWorldIT::fallForever));

    suite->addTest(new CppUnit::TestCaller<CollisionWorldIT>("changePositionOnInactiveBody", &CollisionWorldIT::changePositionOnInactiveBody));
    suite->addTest(new CppUnit::TestCaller<CollisionWorldIT>("changeMomentumOnInactiveBody", &CollisionWorldIT::changeMomentumOnInactiveBody));

    return suite;
}
