#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>

#include <AssertHelper.h>
#include <physics/collision/narrowphase/algorithm/gjk/GJKConvexObjectTest.h>
#include <physics/collision/narrowphase/algorithm/gjk/GJKTestHelper.h>
using namespace urchin;

void GJKConvexObjectTest::separateSphereAndBox() {
    CollisionSphereObject sphere(1.0f, Point3<float>(0.0f, 0.0f, 0.0f));
    CollisionBoxObject aabbox(0.0f, Vector3<float>(0.5f, 0.5f, 0.5f), Point3<float>(1.6f, 0.5f, 0.5f), Quaternion<float>());

    GJKResult<float> result = GJKTestHelper::executeGJK(sphere, aabbox);

    AssertHelper::assertTrue(!result.isCollide());
    AssertHelper::assertFloatEquals(result.getSeparatingDistance(), 0.1f);
}

void GJKConvexObjectTest::overlapSphereAndBox() {
    CollisionSphereObject sphere(1.0f, Point3<float>(0.0f, 0.0f, 0.0f));
    CollisionBoxObject aabbox(0.0f, Vector3<float>(0.5f, 0.5f, 0.5f), Point3<float>(1.4f, 0.5f, 0.5f), Quaternion<float>());

    GJKResult<float> result = GJKTestHelper::executeGJK(sphere, aabbox);

    AssertHelper::assertTrue(result.isCollide());
}

CppUnit::Test* GJKConvexObjectTest::suite() {
    auto* suite = new CppUnit::TestSuite("GJKConvexObjectTest");

    suite->addTest(new CppUnit::TestCaller<GJKConvexObjectTest>("separateSphereAndBox", &GJKConvexObjectTest::separateSphereAndBox));
    suite->addTest(new CppUnit::TestCaller<GJKConvexObjectTest>("overlapSphereAndBox", &GJKConvexObjectTest::overlapSphereAndBox));

    return suite;
}
