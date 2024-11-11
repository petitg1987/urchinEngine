#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>

#include <AssertHelper.h>
#include <physics/object/SupportPointTest.h>
using namespace urchin;

void SupportPointTest::sphereSupportPoint() {
    CollisionSphereObject sphereObject(10.0f, Point3(0.0f, 0.0f, 0.0f));

    AssertHelper::assertPoint3FloatEquals(sphereObject.getSupportPoint(Vector3(1.0f, 0.0f, 0.0f), false), Point3(0.0f, 0.0f, 0.0f));
    AssertHelper::assertPoint3FloatEquals(sphereObject.getSupportPoint(Vector3(1.0f, 0.0f, 0.0f), true), Point3(10.0f, 0.0f, 0.0f));
}

void SupportPointTest::boxSupportPoint() {
    CollisionBoxObject boxObject(0.04f, Vector3(1.0f, 1.0f, 1.0f), Point3(0.0f, 0.0f, 0.0f), Quaternion<float>::rotationZ(2.35619449f));

    AssertHelper::assertPoint3FloatEquals(boxObject.getSupportPoint(Vector3(1.0f, 0.0f, -0.1f), false), Point3(1.41421356237f, 0.0f, -1.0f));
    AssertHelper::assertPoint3FloatEquals(boxObject.getSupportPoint(Vector3(1.0f, 0.0f, 0.1f), false), Point3(1.41421356237f, 0.0f, 1.0f));
    AssertHelper::assertPoint3FloatEquals(boxObject.getSupportPoint(Vector3(-1.0f, 0.0f, -0.1f), false), Point3(-1.41421356237f, 0.0f, -1.0f));
    AssertHelper::assertPoint3FloatEquals(boxObject.getSupportPoint(Vector3(-1.0f, 0.0f, 0.1f), false), Point3(-1.41421356237f, 0.0f, 1.0f));
    AssertHelper::assertPoint3FloatEquals(boxObject.getSupportPoint(Vector3(1.0f, 0.0f, -0.1f), true), Point3(1.47078210487f, 0.0f, -1.04f));
    AssertHelper::assertPoint3FloatEquals(boxObject.getSupportPoint(Vector3(1.0f, 0.0f, 0.1f), true), Point3(1.47078210487f, 0.0f, 1.04f));
    AssertHelper::assertPoint3FloatEquals(boxObject.getSupportPoint(Vector3(-1.0f, 0.0f, -0.1f), true), Point3(-1.47078210487f, 0.0f, -1.04f));
    AssertHelper::assertPoint3FloatEquals(boxObject.getSupportPoint(Vector3(-1.0f, 0.0f, 0.1f), true), Point3(-1.47078210487f, 0.0f, 1.04f));
}

void SupportPointTest::capsuleSupportPoint() {
    CollisionCapsuleObject capsuleObject(0.04f, 0.5f, 5.0f, CapsuleShape<float>::CAPSULE_Y, Point3(1.0f, 1.0f, 1.0f), Quaternion<float>::rotationZ(2.35619449f));

    AssertHelper::assertPoint3FloatEquals(capsuleObject.getSupportPoint(Vector3(1.0f, 0.0f, 0.0f), false), Point3(3.26776695297f, 2.76776695297f, 1.0f));
    AssertHelper::assertPoint3FloatEquals(capsuleObject.getSupportPoint(Vector3(1.0f, 1.0f, 0.0f), false), Point3(3.12132034356f, 3.12132034356f, 1.0f));
    AssertHelper::assertPoint3FloatEquals(capsuleObject.getSupportPoint(Vector3(1.0f, 0.0f, 0.0f), true), Point3(3.30776695297f, 2.76776695297f, 1.0f));
    AssertHelper::assertPoint3FloatEquals(capsuleObject.getSupportPoint(Vector3(1.0f, 1.0f, 0.0f), true), Point3(3.14960461481f, 3.14960461481f, 1.0f));
}

void SupportPointTest::cylinderSupportPoint() {
    CollisionCylinderObject cylinderObject(0.04f, 0.5f, 5.0f, CylinderShape<float>::CYLINDER_Y, Point3(1.0f, 1.0f, 1.0f), Quaternion<float>::rotationZ(2.35619449f));

    AssertHelper::assertPoint3FloatEquals(cylinderObject.getSupportPoint(Vector3(1.0f, 0.0f, 0.0f), false), Point3(3.12132034356f, 2.41421356237f, 1.0f));
    AssertHelper::assertPoint3FloatEquals(cylinderObject.getSupportPoint(Vector3(1.0f, 0.9f, 0.0f), false), Point3(3.12132034356f, 2.41421356237f, 1.0f));
    AssertHelper::assertPoint3FloatEquals(cylinderObject.getSupportPoint(Vector3(1.0f, 1.1f, 0.0f), false), Point3(2.41421356237f, 3.12132034356f, 1.0f));
    AssertHelper::assertPoint3FloatEquals(cylinderObject.getSupportPoint(Vector3(1.0f, 0.0f, 0.0f), true), Point3(3.17788888605f, 2.41421356237f, 1.0f));
    AssertHelper::assertPoint3FloatEquals(cylinderObject.getSupportPoint(Vector3(1.0f, 0.9f, 0.0f), true), Point3(3.17788888605f, 2.41421356237f, 1.0f));
    AssertHelper::assertPoint3FloatEquals(cylinderObject.getSupportPoint(Vector3(1.0f, 1.1f, 0.0f), true), Point3(2.41421356237f, 3.17788888605f, 1.0f));
}

void SupportPointTest::coneSupportPoint() {
    CollisionConeObject coneObject(0.04f, 1.0f, 3.0f, ConeShape<float>::CONE_Y_POSITIVE, Point3(0.25f, 1.0f, 0.0f), Quaternion<float>::rotationZ(-MathValue::PI_FLOAT / 2.0f));

    AssertHelper::assertPoint3FloatEquals(coneObject.getSupportPoint(Vector3(1.0f, 0.0f, 0.0f), false), Point3(2.5f, 1.0f, 0.0f));
    AssertHelper::assertPoint3FloatEquals(coneObject.getSupportPoint(Vector3(2.0f, -1.0f, 0.0f), false), Point3(2.5f, 1.0f, 0.0f));
    AssertHelper::assertPoint3FloatEquals(coneObject.getSupportPoint(Vector3(0.0f, -1.0f, 0.0f), false), Point3(-0.5f, 0.0f, 0.0f));
    AssertHelper::assertPoint3FloatEquals(coneObject.getSupportPoint(Vector3(0.0f, 1.0f, 0.0f), false), Point3(-0.5f, 2.0f, 0.0f));
    AssertHelper::assertPoint3FloatEquals(coneObject.getSupportPoint(Vector3(-1.0f, 0.0f, 0.0f), false), Point3(-0.5f, 1.0f, 0.0f));
    AssertHelper::assertPoint3FloatEquals(coneObject.getSupportPoint(Vector3(0.0f, 1.0f, 1.0f), false), Point3(-0.5f, 1.707107f, 0.707107f));
    AssertHelper::assertPoint3FloatEquals(coneObject.getSupportPoint(Vector3(-1.0f, -2.0f, 0.0f), false), Point3(-0.5f, 0.0f, 0.0f));
    AssertHelper::assertPoint3FloatEquals(coneObject.getSupportPoint(Vector3(0.1f, 1.0f, 0.0f), false), Point3(-0.5f, 2.0f, 0.0f));
}

void SupportPointTest::convexHullSupportPoint() {
    std::vector<Point3<float>> obbPointsWithMargin = {
            Point3(-0.34f, 1.04f, 0.04f), Point3(0.24f, 0.0f, 0.04f), Point3(-0.34f, -1.04f, 0.04f), Point3(-0.84f, 0.0f, 0.04f),
            Point3(-0.34f, 1.04f, -1.04f), Point3(0.24f, 0.0f, -1.04f), Point3(-0.34f, -1.04f, -1.04f), Point3(-0.84f, 0.0f, -1.04f)
    };
    std::vector<Point3<float>> obbPointsWithoutMargin = {
            Point3(-0.3f, 1.0f, 0.0f), Point3(0.2f, 0.0f, 0.0f), Point3(-0.3f, -1.0f, 0.0f), Point3(-0.8f, 0.0f, 0.0f),
            Point3(-0.3f, 1.0f, -1.0f), Point3(0.2f, 0.0f, -1.0f), Point3(-0.3f, -1.0f, -1.0f), Point3(-0.8f, 0.0f, -1.0f)
    };
    CollisionConvexHullObject convexHullObject(0.04f, obbPointsWithMargin, obbPointsWithoutMargin);

    AssertHelper::assertPoint3FloatEquals(convexHullObject.getSupportPoint(Vector3(1.0f, 0.0f, -0.1f), false), Point3(0.2f, 0.0f, -1.0f));
    AssertHelper::assertPoint3FloatEquals(convexHullObject.getSupportPoint(Vector3(1.0f, 0.0f, 0.1f), false), Point3(0.2f, 0.0f, 0.0f));
    AssertHelper::assertPoint3FloatEquals(convexHullObject.getSupportPoint(Vector3(1.0f, 0.0f, -0.1f), true), Point3(0.24f, 0.0f, -1.04f));
    AssertHelper::assertPoint3FloatEquals(convexHullObject.getSupportPoint(Vector3(1.0f, 0.0f, 0.1f), true), Point3(0.24f, 0.0f, 0.04f));
}

CppUnit::Test* SupportPointTest::suite() {
    auto* suite = new CppUnit::TestSuite("SupportPointTest");

    suite->addTest(new CppUnit::TestCaller("sphereSupportPoint", &SupportPointTest::sphereSupportPoint));
    suite->addTest(new CppUnit::TestCaller("boxSupportPoint", &SupportPointTest::boxSupportPoint));
    suite->addTest(new CppUnit::TestCaller("capsuleSupportPoint", &SupportPointTest::capsuleSupportPoint));
    suite->addTest(new CppUnit::TestCaller("cylinderSupportPoint", &SupportPointTest::cylinderSupportPoint));
    suite->addTest(new CppUnit::TestCaller("coneSupportPoint", &SupportPointTest::coneSupportPoint));
    suite->addTest(new CppUnit::TestCaller("convexHullSupportPoint", &SupportPointTest::convexHullSupportPoint));

    return suite;
}
