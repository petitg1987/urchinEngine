#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"

#include "AssertHelper.h"
#include "physics/object/SupportPointTest.h"
using namespace urchin;

void SupportPointTest::sphereSupportPoint() {
    CollisionSphereObject sphereObject(10.0, Point3<float>(0.0, 0.0, 0.0));

    AssertHelper::assertPoint3FloatEquals(sphereObject.getSupportPoint(Vector3<float>(1.0, 0.0, 0.0), false), Point3<float>(0.0, 0.0, 0.0));
    AssertHelper::assertPoint3FloatEquals(sphereObject.getSupportPoint(Vector3<float>(1.0, 0.0, 0.0), true), Point3<float>(10.0, 0.0, 0.0));
}

void SupportPointTest::boxSupportPoint() {
    CollisionBoxObject boxObject(0.04f, Vector3<float>(1.0, 1.0, 1.0), Point3<float>(0.0, 0.0, 0.0),
            Quaternion<float>(Vector3<float>(0.0, 0.0, 1.0), 2.35619449));

    AssertHelper::assertPoint3FloatEquals(boxObject.getSupportPoint(Vector3<float>(1.0, 0.0, -0.1), false), Point3<float>(1.41421356237, 0.0, -1.0));
    AssertHelper::assertPoint3FloatEquals(boxObject.getSupportPoint(Vector3<float>(1.0, 0.0, 0.1), false), Point3<float>(1.41421356237, 0.0, 1.0));
    AssertHelper::assertPoint3FloatEquals(boxObject.getSupportPoint(Vector3<float>(-1.0, 0.0, -0.1), false), Point3<float>(-1.41421356237, 0.0, -1.0));
    AssertHelper::assertPoint3FloatEquals(boxObject.getSupportPoint(Vector3<float>(-1.0, 0.0, 0.1), false), Point3<float>(-1.41421356237, 0.0, 1.0));
    AssertHelper::assertPoint3FloatEquals(boxObject.getSupportPoint(Vector3<float>(1.0, 0.0, -0.1), true), Point3<float>(1.47078210487, 0.0, -1.04));
    AssertHelper::assertPoint3FloatEquals(boxObject.getSupportPoint(Vector3<float>(1.0, 0.0, 0.1), true), Point3<float>(1.47078210487, 0.0, 1.04));
    AssertHelper::assertPoint3FloatEquals(boxObject.getSupportPoint(Vector3<float>(-1.0, 0.0, -0.1), true), Point3<float>(-1.47078210487, 0.0, -1.04));
    AssertHelper::assertPoint3FloatEquals(boxObject.getSupportPoint(Vector3<float>(-1.0, 0.0, 0.1), true), Point3<float>(-1.47078210487, 0.0, 1.04));
}

void SupportPointTest::capsuleSupportPoint() {
    CollisionCapsuleObject capsuleObject(0.04f, 0.5f, 5.0f, CapsuleShape<float>::CAPSULE_Y, Point3<float>(1.0, 1.0, 1.0),
            Quaternion<float>(Vector3<float>(0.0, 0.0, 1.0), 2.35619449));

    AssertHelper::assertPoint3FloatEquals(capsuleObject.getSupportPoint(Vector3<float>(1.0, 0.0, 0.0), false), Point3<float>(3.26776695297, 2.76776695297, 1.0));
    AssertHelper::assertPoint3FloatEquals(capsuleObject.getSupportPoint(Vector3<float>(1.0, 1.0, 0.0), false), Point3<float>(3.12132034356, 3.12132034356, 1.0));
    AssertHelper::assertPoint3FloatEquals(capsuleObject.getSupportPoint(Vector3<float>(1.0, 0.0, 0.0), true), Point3<float>(3.30776695297, 2.76776695297, 1.0));
    AssertHelper::assertPoint3FloatEquals(capsuleObject.getSupportPoint(Vector3<float>(1.0, 1.0, 0.0), true), Point3<float>(3.14960461481, 3.14960461481, 1.0));
}

void SupportPointTest::cylinderSupportPoint() {
    CollisionCylinderObject cylinderObject(0.04f, 0.5f, 5.0f, CylinderShape<float>::CYLINDER_Y, Point3<float>(1.0, 1.0, 1.0),
            Quaternion<float>(Vector3<float>(0.0, 0.0, 1.0), 2.35619449));

    AssertHelper::assertPoint3FloatEquals(cylinderObject.getSupportPoint(Vector3<float>(1.0, 0.0, 0.0), false), Point3<float>(3.12132034356, 2.41421356237, 1.0));
    AssertHelper::assertPoint3FloatEquals(cylinderObject.getSupportPoint(Vector3<float>(1.0, 0.9, 0.0), false), Point3<float>(3.12132034356, 2.41421356237, 1.0));
    AssertHelper::assertPoint3FloatEquals(cylinderObject.getSupportPoint(Vector3<float>(1.0, 1.1, 0.0), false), Point3<float>(2.41421356237, 3.12132034356, 1.0));
    AssertHelper::assertPoint3FloatEquals(cylinderObject.getSupportPoint(Vector3<float>(1.0, 0.0, 0.0), true), Point3<float>(3.17788888605, 2.41421356237, 1.0));
    AssertHelper::assertPoint3FloatEquals(cylinderObject.getSupportPoint(Vector3<float>(1.0, 0.9, 0.0), true), Point3<float>(3.17788888605, 2.41421356237, 1.0));
    AssertHelper::assertPoint3FloatEquals(cylinderObject.getSupportPoint(Vector3<float>(1.0, 1.1, 0.0), true), Point3<float>(2.41421356237, 3.17788888605, 1.0));
}

void SupportPointTest::coneSupportPoint() {
    CollisionConeObject coneObject(0.04f, 1.0f, 3.0f, ConeShape<float>::CONE_Y_POSITIVE, Point3<float>(0.25, 1.0, 0.0),
            Quaternion<float>(Vector3<float>(0.0, 0.0, -1.0), MathValue::PI_FLOAT/2.0f));

    AssertHelper::assertPoint3FloatEquals(coneObject.getSupportPoint(Vector3<float>(1.0, 0.0, 0.0), false), Point3<float>(2.5, 1.0, 0.0));
    AssertHelper::assertPoint3FloatEquals(coneObject.getSupportPoint(Vector3<float>(2.0, -1.0, 0.0), false), Point3<float>(2.5, 1.0, 0.0));
    AssertHelper::assertPoint3FloatEquals(coneObject.getSupportPoint(Vector3<float>(0.0, -1.0, 0.0), false), Point3<float>(-0.5, 0.0, 0.0));
    AssertHelper::assertPoint3FloatEquals(coneObject.getSupportPoint(Vector3<float>(0.0, 1.0, 0.0), false), Point3<float>(-0.5, 2.0, 0.0));
    AssertHelper::assertPoint3FloatEquals(coneObject.getSupportPoint(Vector3<float>(-1.0, 0.0, 0.0), false), Point3<float>(-0.5, 1.0, 0.0));
    AssertHelper::assertPoint3FloatEquals(coneObject.getSupportPoint(Vector3<float>(0.0, 1.0, 1.0), false), Point3<float>(-0.5, 1.707107, 0.707107));
    AssertHelper::assertPoint3FloatEquals(coneObject.getSupportPoint(Vector3<float>(-1.0, -2.0, 0.0), false), Point3<float>(-0.5, 0.0, 0.0));
    AssertHelper::assertPoint3FloatEquals(coneObject.getSupportPoint(Vector3<float>(0.1, 1.0, 0.0), false), Point3<float>(-0.5, 2.0, 0.0));
}

void SupportPointTest::convexHullSupportPoint() {
    Point3<float> obbPointsWithMarginTab[] = {
            Point3<float>(-0.34, 1.04, 0.04), Point3<float>(0.24, 0.0, 0.04), Point3<float>(-0.34, -1.04, 0.04), Point3<float>(-0.84, 0.0, 0.04),
            Point3<float>(-0.34, 1.04, -1.04), Point3<float>(0.24, 0.0, -1.04), Point3<float>(-0.34, -1.04, -1.04), Point3<float>(-0.84, 0.0, -1.04)
    };
    Point3<float> obbPointsWithoutMarginTab[] = {
            Point3<float>(-0.3, 1.0, 0.0), Point3<float>(0.2, 0.0, 0.0), Point3<float>(-0.3, -1.0, 0.0), Point3<float>(-0.8, 0.0, 0.0),
            Point3<float>(-0.3, 1.0, -1.0), Point3<float>(0.2, 0.0, -1.0), Point3<float>(-0.3, -1.0, -1.0), Point3<float>(-0.8, 0.0, -1.0)
    };

    std::vector<Point3<float>> obbPointsWithMargin(obbPointsWithMarginTab, obbPointsWithMarginTab+sizeof(obbPointsWithMarginTab)/sizeof(Point3<float>));
    std::vector<Point3<float>> obbPointsWithoutMargin(obbPointsWithoutMarginTab, obbPointsWithoutMarginTab+sizeof(obbPointsWithoutMarginTab)/sizeof(Point3<float>));
    CollisionConvexHullObject convexHullObject(0.04f, obbPointsWithMargin, obbPointsWithoutMargin);

    AssertHelper::assertPoint3FloatEquals(convexHullObject.getSupportPoint(Vector3<float>(1.0, 0.0, -0.1), false), Point3<float>(0.2, 0.0, -1.0));
    AssertHelper::assertPoint3FloatEquals(convexHullObject.getSupportPoint(Vector3<float>(1.0, 0.0, 0.1), false), Point3<float>(0.2, 0.0, 0.0));
    AssertHelper::assertPoint3FloatEquals(convexHullObject.getSupportPoint(Vector3<float>(1.0, 0.0, -0.1), true), Point3<float>(0.24, 0.0, -1.04));
    AssertHelper::assertPoint3FloatEquals(convexHullObject.getSupportPoint(Vector3<float>(1.0, 0.0, 0.1), true), Point3<float>(0.24, 0.0, 0.04));
}

CppUnit::Test* SupportPointTest::suite() {
    auto* suite = new CppUnit::TestSuite("SupportPointTest");

    suite->addTest(new CppUnit::TestCaller<SupportPointTest>("sphereSupportPoint", &SupportPointTest::sphereSupportPoint));
    suite->addTest(new CppUnit::TestCaller<SupportPointTest>("boxSupportPoint", &SupportPointTest::boxSupportPoint));
    suite->addTest(new CppUnit::TestCaller<SupportPointTest>("capsuleSupportPoint", &SupportPointTest::capsuleSupportPoint));
    suite->addTest(new CppUnit::TestCaller<SupportPointTest>("cylinderSupportPoint", &SupportPointTest::cylinderSupportPoint));
    suite->addTest(new CppUnit::TestCaller<SupportPointTest>("coneSupportPoint", &SupportPointTest::coneSupportPoint));
    suite->addTest(new CppUnit::TestCaller<SupportPointTest>("convexHullSupportPoint", &SupportPointTest::convexHullSupportPoint));

    return suite;
}
