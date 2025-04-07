#include <cppunit/extensions/HelperMacros.h>
#include <UrchinCommon.h>

#include <common/math/geometry/3d/object/OBBoxTest.h>
#include <AssertHelper.h>
using namespace urchin;

void OBBoxTest::toProjectionMatrix() { //TODO adapt
    OBBox box(Vector3(4.0f, 4.0f, 4.0f), Point3(1.0f, 1.0f, 1.0f), Quaternion<float>::rotationX(0.0f));

    Matrix4<float> projectionMatrix = box.toProjectionMatrix();

    Point4<float> transformedPointMin = projectionMatrix * Point4(-3.0f, -3.0f, -3.0f, 1.0f);
    AssertHelper::assertPoint3FloatEquals(transformedPointMin.toPoint3(), Point3(-1.0f, 1.0f, 1.0f));

    Point4<float> transformedPointMax = projectionMatrix * Point4(5.0f, 5.0f, 5.0f, 1.0f);
    AssertHelper::assertPoint3FloatEquals(transformedPointMax.toPoint3(), Point3(1.0f, -1.0f, 0.0f));

    Point4<float> transformedPointCenter = projectionMatrix * Point4(box.getCenterOfMass(), 1.0f);
    AssertHelper::assertPoint3FloatEquals(transformedPointCenter.toPoint3(), Point3(0.0f, 0.0f, 0.5f));
}

void OBBoxTest::noIntersection() {
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
    OBBox oBBox(Vector3(1.0f, 1.0f, 1.0f), Point3(0.0f, 0.0f, 0.0f), Quaternion<float>::rotationZ(0.78539816339f) /* 45 degrees */);
    Line3D line(Point3(0.0f, 3.0f, 0.0f), Point3(0.0f, -3.0f, 0.0f));

    bool hasIntersection = false;
    Point3<float> intersectionPoint = oBBox.intersectPoint(line, hasIntersection);

    AssertHelper::assertTrue(hasIntersection);
    AssertHelper::assertPoint3FloatEquals(intersectionPoint, Point3(0.0f, 1.414213f, 0.0f));
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

    suite->addTest(new CppUnit::TestCaller("toProjectionMatrix", &OBBoxTest::toProjectionMatrix));

    suite->addTest(new CppUnit::TestCaller("noIntersection", &OBBoxTest::noIntersection));
    suite->addTest(new CppUnit::TestCaller("leftToRightLineIntersection", &OBBoxTest::leftToRightLineIntersection));
    suite->addTest(new CppUnit::TestCaller("rightToLeftLineIntersection", &OBBoxTest::rightToLeftLineIntersection));
    suite->addTest(new CppUnit::TestCaller("farToNearLineIntersection", &OBBoxTest::farToNearLineIntersection));
    suite->addTest(new CppUnit::TestCaller("nearToFarLineIntersection", &OBBoxTest::nearToFarLineIntersection));
    suite->addTest(new CppUnit::TestCaller("bottomRightToTopLeftLineIntersection", &OBBoxTest::bottomRightToTopLeftLineIntersection));
    suite->addTest(new CppUnit::TestCaller("oBBoxObliqueIntersection", &OBBoxTest::oBBoxObliqueIntersection));

    suite->addTest(new CppUnit::TestCaller("matrixMultiplication", &OBBoxTest::matrixMultiplication));

    return suite;
}
