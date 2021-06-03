#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>

#include <AssertHelper.h>
#include <physics/body/BodyManagerTest.h>
using namespace urchin;

void BodyManagerTest::addAndRemoveBody() {
    auto bodyManager = std::make_unique<BodyManager>();
    std::shared_ptr<CollisionBoxShape> groundShape = std::make_shared<CollisionBoxShape>(Vector3<float>(1000.0f, 0.5f, 1000.0f));
    auto* groundBody = new RigidBody("ground", PhysicsTransform(Point3<float>(0.0f, -0.5f, 0.0f), Quaternion<float>()), groundShape);
    bodyManager->addBody(groundBody);
    bodyManager->removeBody(groundBody);
    AssertHelper::assertUnsignedInt(bodyManager->getBodies().size(), 0);

    bodyManager->refreshBodies();

    AssertHelper::assertUnsignedInt(bodyManager->getBodies().size(), 0);
}

CppUnit::Test* BodyManagerTest::suite() {
    auto* suite = new CppUnit::TestSuite("BodyManagerTest");

    suite->addTest(new CppUnit::TestCaller<BodyManagerTest>("addSameBodyThrice", &BodyManagerTest::addAndRemoveBody));

    return suite;
}

