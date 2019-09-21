#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <memory>
#include <cstdio>

#include "physics/it/FallingObjectIT.h"
#include "AssertHelper.h"
#include "UrchinPhysicsEngine.h"
#include "utils/LogsUtils.h"
using namespace urchin;

void FallingObjectIT::fallOnPlane()
{
    std::shared_ptr<CollisionBoxShape> planeShape = std::make_shared<CollisionBoxShape>(Vector3<float>(1000.0f, 0.5f, 1000.0f));
    auto *planeBody = new RigidBody("plane", Transform<float>(Point3<float>(0.0f, -0.5f, 0.0f), Quaternion<float>(), 1.0f), planeShape);

    std::shared_ptr<CollisionBoxShape> cubeShape = std::make_shared<CollisionBoxShape>(Vector3<float>(0.5f, 0.5f, 0.5f));
    auto *cubeBody = new RigidBody("cube", Transform<float>(Point3<float>(0.0f, 5.0f, 0.0f), Quaternion<float>(), 1.0f), cubeShape);
    cubeBody->setMass(10.0f);

    auto *bodyManager = new BodyManager();
    bodyManager->addBody(planeBody);
    bodyManager->addBody(cubeBody);
    auto *collisionWorld = new CollisionWorld(bodyManager);

    for(std::size_t i=0; i<150; ++i)
    {
        collisionWorld->process(1.0f / 60.0f, Vector3<float>(0.0f, -9.81f, 0.0f));
    }

    AssertHelper::assertFloatEquals(cubeBody->getTransform().getPosition().Y, 0.5f, 0.1f);
}

void FallingObjectIT::fallForever()
{
    LogsUtils::emptyLogFile();

    std::shared_ptr<CollisionBoxShape> planeShape = std::make_shared<CollisionBoxShape>(Vector3<float>(1000.0f, 0.5f, 1000.0f));
    auto *planeBody = new RigidBody("plane", Transform<float>(Point3<float>(0.0f, -0.5f, 0.0f), Quaternion<float>(), 1.0f), planeShape);

    std::shared_ptr<CollisionBoxShape> cubeShape = std::make_shared<CollisionBoxShape>(Vector3<float>(0.5f, 0.5f, 0.5f));
    auto *cubeBody = new RigidBody("cube", Transform<float>(Point3<float>(1100.0f, 5.0f, 0.0f), Quaternion<float>(), 1.0f), cubeShape);
    cubeBody->setMass(10.0f); //non-static

    auto *bodyManager = new BodyManager();
    bodyManager->addBody(cubeBody);
    bodyManager->addBody(planeBody);
    auto *collisionWorld = new CollisionWorld(bodyManager);

    for(std::size_t i=0; i<500; ++i)
    {
        collisionWorld->process(1.0f / 60.0f, Vector3<float>(0.0f, -9.81f, 0.0f));
    }

    AssertHelper::assertTrue(cubeBody->getTransform().getPosition().Y > -3.0f, "Check cube doesn't fall forever");
    AssertHelper::assertTrue(LogsUtils::logFileContain("(WW) Body cube is below the limit of"), "Log must contain warning about body falling forever");
    LogsUtils::emptyLogFile();
}

CppUnit::Test *FallingObjectIT::suite()
{
    auto *suite = new CppUnit::TestSuite("FallingObjectIT");

    suite->addTest(new CppUnit::TestCaller<FallingObjectIT>("fallOnPlane", &FallingObjectIT::fallOnPlane));
    suite->addTest(new CppUnit::TestCaller<FallingObjectIT>("fallForever", &FallingObjectIT::fallForever));

    return suite;
}
