#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>

#include <AssertHelper.h>
#include <physics/body/BodyContainerTest.h>
using namespace urchin;

void BodyContainerTest::addAndRemoveBody() {
    auto bodyContainer = std::make_unique<BodyContainer>();
    std::unique_ptr<CollisionBoxShape> groundShape = std::make_unique<CollisionBoxShape>(Vector3<float>(1000.0f, 0.5f, 1000.0f));
    auto groundBody = std::make_shared<RigidBody>("ground", PhysicsTransform(Point3<float>(0.0f, -0.5f, 0.0f), Quaternion<float>()), std::move(groundShape));
    bodyContainer->addBody(groundBody);
    bodyContainer->removeBody(*groundBody);
    AssertHelper::assertUnsignedInt(bodyContainer->getBodies().size(), 0);

    bodyContainer->refreshBodies();

    AssertHelper::assertUnsignedInt(bodyContainer->getBodies().size(), 0);
}

CppUnit::Test* BodyContainerTest::suite() {
    auto* suite = new CppUnit::TestSuite("BodyContainerTest");

    suite->addTest(new CppUnit::TestCaller<BodyContainerTest>("addSameBodyThrice", &BodyContainerTest::addAndRemoveBody));

    return suite;
}

