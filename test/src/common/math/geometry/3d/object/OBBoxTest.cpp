#include <cppunit/extensions/HelperMacros.h>
#include <UrchinCommon.h>

#include "common/math/geometry/3d/object/OBBoxTest.h"
#include "AssertHelper.h"
using namespace urchin;

void OBBoxTest::noLineIntersection() {
    OBBox oBBox(AABBox(Point3(0.0f, 0.0f, 0.0f), Point3(1.0f, 1.0f, 1.0f)));
    Line3D line(Point3(-2.0f, 4.0f, 4.0f), Point3(2.0f, 1.0f, 1.0f));

    bool hasIntersection = false;
    oBBox.intersectPoint(line, hasIntersection);

    AssertHelper::assertFalse(hasIntersection);
}

void OBBoxTest::leftToRightLineIntersection() {
    OBBox oBBox(AABBox(Point3(0.0f, 0.0f, 0.0f), Point3(1.0f, 1.0f, 1.0f)));
    Line3D line(Point3(-2.0f, 0.5f, 0.5f), Point3(2.0f, 0.5f, 0.5f));

    bool hasIntersection = false;
    Point3<float> intersectionPoint = oBBox.intersectPoint(line, hasIntersection);

    AssertHelper::assertTrue(hasIntersection);
    AssertHelper::assertPoint3FloatEquals(intersectionPoint, Point3(0.0f, 0.5f, 0.5f));
}

void OBBoxTest::rightToLeftLineIntersection() {
    OBBox oBBox(AABBox(Point3(0.0f, 0.0f, 0.0f), Point3(1.0f, 1.0f, 1.0f)));
    Line3D line(Point3(2.0f, 0.5f, 0.5f), Point3(-2.0f, 0.5f, 0.5f));

    bool hasIntersection = false;
    Point3<float> intersectionPoint = oBBox.intersectPoint(line, hasIntersection);

    AssertHelper::assertTrue(hasIntersection);
    AssertHelper::assertPoint3FloatEquals(intersectionPoint, Point3(1.0f, 0.5f, 0.5f));
}

void OBBoxTest::farToNearLineIntersection() {
    Quaternion quaternionY180Degrees(0.0f, 1.0f, 0.0f, -0.00000016f /* this small number lead to a collision on the left plane but not on the right plane */);
    OBBox oBBox(Vector3(3.0f, 1.125f, 0.375f), Point3(-4.875f, 0.75f, -2.25f), quaternionY180Degrees);
    Line3D line(Point3(-3.75f, 0.75f, -7.875f), Point3(-3.75f, 0.75f, -6.875f));

    bool hasIntersection = false;
    Point3<float> intersectionPoint = oBBox.intersectPoint(line, hasIntersection);

    AssertHelper::assertTrue(hasIntersection);
    AssertHelper::assertPoint3FloatEquals(intersectionPoint, Point3(-3.75f, 0.75f, -2.625f));
}

void OBBoxTest::nearToFarLineIntersection() {
    OBBox oBBox(AABBox(Point3(0.0f, 0.0f, 0.0f), Point3(1.0f, 1.0f, 1.0f)));
    Line3D line(Point3(0.5f, 0.5f, 0.5f), Point3(0.5f, 0.5f, -0.5f));

    bool hasIntersection = false;
    Point3<float> intersectionPoint = oBBox.intersectPoint(line, hasIntersection);

    AssertHelper::assertTrue(hasIntersection);
    AssertHelper::assertPoint3FloatEquals(intersectionPoint, Point3(0.5f, 0.5f, 1.0f));
}

void OBBoxTest::bottomRightToTopLeftLineIntersection() {
    OBBox oBBox(AABBox(Point3(0.0f, 0.0f, 0.0f), Point3(1.0f, 1.0f, 1.0f)));
    Line3D line(Point3(0.5f, 0.0f, 0.5f), Point3(0.0f, 0.5f, 0.5f));

    bool hasIntersection = false;
    Point3<float> intersectionPoint = oBBox.intersectPoint(line, hasIntersection);

    AssertHelper::assertTrue(hasIntersection);
    AssertHelper::assertPoint3FloatEquals(intersectionPoint, Point3(0.5f, 0.0f, 0.5f));
}

void OBBoxTest::oBBoxObliqueIntersection() {
    OBBox oBBox(Vector3(1.0f, 1.0f, 1.0f), Point3(0.0f, 0.0f, 0.0f), Quaternion<float>::rotationZ(MathValue::PI_FLOAT / 4.0f));
    Line3D line(Point3(0.0f, 3.0f, 0.0f), Point3(0.0f, -3.0f, 0.0f));

    bool hasIntersection = false;
    Point3<float> intersectionPoint = oBBox.intersectPoint(line, hasIntersection);

    AssertHelper::assertTrue(hasIntersection);
    AssertHelper::assertPoint3FloatEquals(intersectionPoint, Point3(0.0f, 1.414213f, 0.0f));
}

void OBBoxTest::pointInsideNearToCorner() {
    OBBox oBBox(Vector3(1.0f, 1.0f, 1.0f), Point3(0.0f, 2.0f, 0.0f), Quaternion<float>::rotationZ(0.0f));
    Point3 point(0.99f, 2.99f, 0.99f);

    bool pointIsInside = oBBox.collideWithPoint(point);

    AssertHelper::assertTrue(pointIsInside);
}

void OBBoxTest::pointOutsideNearToCorner() {
    OBBox oBBox(Vector3(1.0f, 1.0f, 1.0f), Point3(0.0f, 2.0f, 0.0f), Quaternion<float>::rotationZ(0.0f));
    Point3 point(1.01f, 2.99f, 0.99f);

    bool pointIsInside = oBBox.collideWithPoint(point);

    AssertHelper::assertFalse(pointIsInside);
}

void OBBoxTest::pointInsideNearToRightSide() {
    OBBox oBBox(Vector3(2.0f, 2.0f, 1.0f), Point3(2.0f, 2.0f, 0.0f), Quaternion<float>::rotationY(0.0f));
    Point3 point(3.5f, 2.0f, 0.0f);

    bool pointIsInside = oBBox.collideWithPoint(point);

    AssertHelper::assertTrue(pointIsInside);
}

void OBBoxTest::pointOutsideNearToRightSide() {
    OBBox oBBox(Vector3(2.0f, 2.0f, 1.0f), Point3(2.0f, 2.0f, 0.0f), Quaternion<float>::rotationY(MathValue::PI_FLOAT / 4.0f));
    Point3 point(3.5f, 2.0f, 0.0f);

    bool pointIsInside = oBBox.collideWithPoint(point);

    AssertHelper::assertFalse(pointIsInside);
}

void OBBoxTest::matrixMultiplication() {
    OBBox oBBox(Vector3(2.0f, 3.0f, 1.0f), Point3(5.0f, 0.0f, 0.0f), Quaternion<float>::rotationX(MathValue::PI_FLOAT / 2.0f));

    OBBox oBBoxTranslated = Matrix4<float>::buildTranslation(0.0f, 10.0f, 0.0f) * oBBox;
    OBBox oBBoxRotated = Matrix4<float>::buildRotationZ(MathValue::PI_FLOAT / 2.0f) * oBBox;

    AssertHelper::assertPoint3FloatEquals(oBBoxRotated.getPoint(0), Point3(1.0f, 7.0f, 3.0f));
    AssertHelper::assertPoint3FloatEquals(oBBoxRotated.getPoint(1), Point3(-1.0f, 7.0f, 3.0f));
    AssertHelper::assertPoint3FloatEquals(oBBoxRotated.getPoint(2), Point3(1.0f, 7.0f, -3.0f));
    AssertHelper::assertPoint3FloatEquals(oBBoxRotated.getPoint(3), Point3(-1.0f, 7.0f, -3.0f));
    AssertHelper::assertPoint3FloatEquals(oBBoxRotated.getPoint(4), Point3(1.0f, 3.0f, 3.0f));
    AssertHelper::assertPoint3FloatEquals(oBBoxRotated.getPoint(5), Point3(-1.0f, 3.0f, 3.0f));
    AssertHelper::assertPoint3FloatEquals(oBBoxRotated.getPoint(6), Point3(1.0f, 3.0f, -3.0f));
    AssertHelper::assertPoint3FloatEquals(oBBoxRotated.getPoint(7), Point3(-1.0f, 3.0f, -3.0f));
}

CppUnit::Test* OBBoxTest::suite() {
    auto* suite = new CppUnit::TestSuite("OBBoxTest");

    suite->addTest(new CppUnit::TestCaller("noLineIntersection", &OBBoxTest::noLineIntersection));
    suite->addTest(new CppUnit::TestCaller("leftToRightLineIntersection", &OBBoxTest::leftToRightLineIntersection));
    suite->addTest(new CppUnit::TestCaller("rightToLeftLineIntersection", &OBBoxTest::rightToLeftLineIntersection));
    suite->addTest(new CppUnit::TestCaller("farToNearLineIntersection", &OBBoxTest::farToNearLineIntersection));
    suite->addTest(new CppUnit::TestCaller("nearToFarLineIntersection", &OBBoxTest::nearToFarLineIntersection));
    suite->addTest(new CppUnit::TestCaller("bottomRightToTopLeftLineIntersection", &OBBoxTest::bottomRightToTopLeftLineIntersection));
    suite->addTest(new CppUnit::TestCaller("oBBoxObliqueIntersection", &OBBoxTest::oBBoxObliqueIntersection));

    suite->addTest(new CppUnit::TestCaller("pointInsideNearToCorner", &OBBoxTest::pointInsideNearToCorner));
    suite->addTest(new CppUnit::TestCaller("pointOutsideNearToCorner", &OBBoxTest::pointOutsideNearToCorner));
    suite->addTest(new CppUnit::TestCaller("pointInsideNearToRightSide", &OBBoxTest::pointInsideNearToRightSide));
    suite->addTest(new CppUnit::TestCaller("pointOutsideNearToRightSide", &OBBoxTest::pointOutsideNearToRightSide));

    suite->addTest(new CppUnit::TestCaller("matrixMultiplication", &OBBoxTest::matrixMultiplication));

    return suite;
}
