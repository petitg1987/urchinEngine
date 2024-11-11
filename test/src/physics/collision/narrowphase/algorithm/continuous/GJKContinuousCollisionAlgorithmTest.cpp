#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

#include <AssertHelper.h>
#include <physics/collision/narrowphase/algorithm/continuous/GJKContinuousCollisionAlgorithmTest.h>
using namespace urchin;

void GJKContinuousCollisionAlgorithmTest::rayTest() { //test created from error logs where fail occurred with MAX_ITERATION=25 / TERMINATION_TOLERANCE=0.0001f
    auto fakeBody = std::make_unique<RigidBody>("fake", PhysicsTransform(Point3<float>(), Quaternion<float>()), std::make_unique<CollisionBoxShape>(Vector3(0.5f, 0.5f, 0.5f)));
    CollisionSphereShape pointShape(0.0f);
    PhysicsTransform rayFrom(Point3(6.24549675f, 1.32499599f, -5.62802887f));
    PhysicsTransform rayTo(Point3(10.297411f, 0.845154405f, -20.0624237f));
    TemporalObject rayCastObject(pointShape, 0, rayFrom, rayTo);
    CollisionBoxShape boxShape(Vector3(4.46000004f + 0.0399999991f, 2.96000004f + 0.0399999991f, 0.335000008f + 0.0399999991f));
    PhysicsTransform boxFrom(Point3(8.625f, 1.875f, -17.25f), Quaternion(0.0f, 1.0f, 0.0f, -1.62920685e-07f));
    PhysicsTransform boxTo(Point3(8.625f, 1.875f, -17.25f), Quaternion(0.0f, 1.0f, 0.0f, -1.62920685e-07f));
    TemporalObject boxObject(boxShape, 0, boxFrom, boxTo);

    ContinuousCollisionResult<float> continuousCollisionResult = GJKContinuousCollisionAlgorithm<double, float>().calculateTimeOfImpact(rayCastObject, boxObject, std::move(fakeBody));

    AssertHelper::assertTrue(continuousCollisionResult.hasResult());
    AssertHelper::assertFloatEquals(continuousCollisionResult.getTimeToHit(), 0.779051125f, 0.001f);
}

CppUnit::Test* GJKContinuousCollisionAlgorithmTest::suite() {
    auto* suite = new CppUnit::TestSuite("GJKContinuousCollisionAlgorithmTest");

    suite->addTest(new CppUnit::TestCaller("rayTest", &GJKContinuousCollisionAlgorithmTest::rayTest));

    return suite;
}
