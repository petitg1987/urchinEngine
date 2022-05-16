#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>

#include <AssertHelper.h>
#include <physics/collision/narrowphase/algorithm/epa/EPAConvexObjectTest.h>
#include <physics/collision/narrowphase/algorithm/epa/EPATestHelper.h>
using namespace urchin;

void EPAConvexObjectTest::overlapSphereAndBox() {
    CollisionSphereObject sphere(1.0f, Point3<float>(0.0f, 0.0f, 0.0f));
    CollisionBoxObject aabbox(0.0f, Vector3<float>(0.5f, 0.5f, 0.5f), Point3<float>(1.4f, 0.5f, 0.5f), Quaternion<float>());

    EPAResult<float> resultEpa = EPATestHelper::executeEPA(sphere, aabbox);

    float epsilon = 0.05f; //high epsilon used because curved shapes are very bad case for EPA
    AssertHelper::assertTrue(resultEpa.isCollide());
    AssertHelper::assertFloatEquals(resultEpa.getPenetrationDepth(), 0.1f);
    AssertHelper::assertVector3FloatEquals(resultEpa.getNormal(), Vector3<float>(1.0f, 0.0f, 0.0f), epsilon);
    AssertHelper::assertFloatEquals(resultEpa.getContactPointA().vector(resultEpa.getContactPointB()).length(), 0.1f, epsilon);
    AssertHelper::assertPoint3FloatEquals(resultEpa.getContactPointA(), Point3<float>(1.0f, 0.0f, 0.0f), epsilon);
    AssertHelper::assertPoint3FloatEquals(resultEpa.getContactPointB(), Point3<float>(0.9f, 0.0f, 0.0f), epsilon);
}

void EPAConvexObjectTest::overlapCapsuleAndTriangle() {
    CollisionCapsuleObject capsule(0.0f, 0.25f, 1.0f, CapsuleShape<float>::CAPSULE_Y, Point3<float>(0.0f, 0.2f, 0.0f), Quaternion<float>());
    CollisionTriangleObject triangle(0.0f, Point3<float>(0.0f, 0.0f, 0.0f), Point3<float>(-2.0f, 0.0f, -2.0f), Point3<float>(-2.0f, 0.0f, 0.0f));

    EPAResult<float> resultEpa = EPATestHelper::executeEPA(capsule, triangle);

    AssertHelper::assertTrue(resultEpa.isCollide());
    AssertHelper::assertFloatEquals(resultEpa.getPenetrationDepth(), 0.55f);
    AssertHelper::assertVector3FloatEquals(resultEpa.getNormal(), Vector3<float>(0.0f, -1.0f, 0.0f));
    AssertHelper::assertPoint3FloatEquals(resultEpa.getContactPointA(), Point3<float>(0.0f, -0.55f, 0.0f));
    AssertHelper::assertPoint3FloatEquals(resultEpa.getContactPointB(), Point3<float>(0.0f, 0.0f, 0.0f));
}

void EPAConvexObjectTest::overlapTriangleAndCapsule() {
    CollisionTriangleObject triangle(0.0f, Point3<float>(0.0f, 0.0f, 0.0f), Point3<float>(-2.0f, 0.0f, -2.0f), Point3<float>(-2.0f, 0.0f, 0.0f));
    CollisionCapsuleObject capsule(0.0f, 0.25f, 1.0f, CapsuleShape<float>::CAPSULE_Y, Point3<float>(0.0f, 0.2f, 0.0f), Quaternion<float>());

    EPAResult<float> resultEpa = EPATestHelper::executeEPA(triangle, capsule);

    AssertHelper::assertTrue(resultEpa.isCollide());
    AssertHelper::assertFloatEquals(resultEpa.getPenetrationDepth(), 0.55f);
    AssertHelper::assertVector3FloatEquals(resultEpa.getNormal(), Vector3<float>(0.0f, 1.0f, 0.0f));
    AssertHelper::assertPoint3FloatEquals(resultEpa.getContactPointA(), Point3<float>(0.0f, 0.0f, 0.0f));
    AssertHelper::assertPoint3FloatEquals(resultEpa.getContactPointB(), Point3<float>(0.0f, -0.55f, 0.0f));
}

CppUnit::Test* EPAConvexObjectTest::suite() {
    auto* suite = new CppUnit::TestSuite("EPAConvexObjectTest");

    suite->addTest(new CppUnit::TestCaller<EPAConvexObjectTest>("overlapSphereAndBox", &EPAConvexObjectTest::overlapSphereAndBox));
    suite->addTest(new CppUnit::TestCaller<EPAConvexObjectTest>("overlapCapsuleAndTriangle", &EPAConvexObjectTest::overlapCapsuleAndTriangle));
    suite->addTest(new CppUnit::TestCaller<EPAConvexObjectTest>("overlapTriangleAndCapsule", &EPAConvexObjectTest::overlapTriangleAndCapsule));

    return suite;
}
