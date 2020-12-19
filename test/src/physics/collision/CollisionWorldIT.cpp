#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <memory>
#include <cstdio>
#include "UrchinPhysicsEngine.h"

#include "physics/collision/CollisionWorldIT.h"
#include "AssertHelper.h"
using namespace urchin;

void CollisionWorldIT::fallOnPlane() {
    std::shared_ptr<CollisionBoxShape> planeShape = std::make_shared<CollisionBoxShape>(Vector3<float>(1000.0f, 0.5f, 1000.0f));
    auto* planeBody = new RigidBody("plane", PhysicsTransform(Point3<float>(0.0f, -0.5f, 0.0f), Quaternion<float>()), planeShape);

    std::shared_ptr<CollisionBoxShape> cubeShape = std::make_shared<CollisionBoxShape>(Vector3<float>(0.5f, 0.5f, 0.5f));
    auto* cubeBody = new RigidBody("cube", PhysicsTransform(Point3<float>(0.0f, 5.0f, 0.0f), Quaternion<float>()), cubeShape);
    cubeBody->setMass(10.0f);

    auto* bodyManager = new BodyManager();
    bodyManager->addBody(planeBody);
    bodyManager->addBody(cubeBody);
    auto* collisionWorld = new CollisionWorld(bodyManager);

    for (std::size_t i = 0; i < 150; ++i) {
        collisionWorld->process(1.0f / 60.0f, Vector3<float>(0.0f, -9.81f, 0.0f));
    }

    AssertHelper::assertFloatEquals(cubeBody->getTransform().getPosition().Y, 0.5f, 0.1f);
    AssertHelper::assertTrue(!cubeBody->isActive(), "Body must become inactive when it doesn't move");

    delete collisionWorld;
    delete bodyManager;
}

void CollisionWorldIT::fallForever() {
    if (!Logger::instance()->retrieveContent(std::numeric_limits<unsigned long>::max()).empty()) {
        throw std::runtime_error("Log file must be emptied before start this test.");
    }

    std::shared_ptr<CollisionBoxShape> planeShape = std::make_shared<CollisionBoxShape>(Vector3<float>(1000.0f, 0.5f, 1000.0f));
    auto* planeBody = new RigidBody("plane", PhysicsTransform(Point3<float>(0.0f, -0.5f, 0.0f), Quaternion<float>()), planeShape);

    std::shared_ptr<CollisionBoxShape> cubeShape = std::make_shared<CollisionBoxShape>(Vector3<float>(0.5f, 0.5f, 0.5f));
    auto* cubeBody = new RigidBody("cube", PhysicsTransform(Point3<float>(1100.0f, 5.0f, 0.0f), Quaternion<float>()), cubeShape);
    cubeBody->setMass(10.0f); //non-static

    auto* bodyManager = new BodyManager();
    bodyManager->addBody(cubeBody);
    bodyManager->addBody(planeBody);
    auto* collisionWorld = new CollisionWorld(bodyManager);

    for (std::size_t i = 0; i < 500; ++i) {
        collisionWorld->process(1.0f / 60.0f, Vector3<float>(0.0f, -9.81f, 0.0f));
    }

    AssertHelper::assertTrue(cubeBody->getTransform().getPosition().Y > -4.0f, "Check cube doesn't fall forever");
    AssertHelper::assertTrue(!cubeBody->isActive(), "Body must become inactive when it goes outside the world limits");
    std::string logValue = Logger::instance()->retrieveContent(std::numeric_limits<unsigned long>::max());
    AssertHelper::assertTrue(logValue.find("(WW) Body cube is below the limit of") != std::string::npos);

    Logger::instance()->purge();
    delete collisionWorld;
    delete bodyManager;
}

CppUnit::Test* CollisionWorldIT::suite() {
    auto* suite = new CppUnit::TestSuite("FallingObjectIT");

    suite->addTest(new CppUnit::TestCaller<CollisionWorldIT>("fallOnPlane", &CollisionWorldIT::fallOnPlane));
    suite->addTest(new CppUnit::TestCaller<CollisionWorldIT>("fallForever", &CollisionWorldIT::fallForever));

    return suite;
}
