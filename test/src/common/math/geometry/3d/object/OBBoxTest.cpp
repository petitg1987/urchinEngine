#include <cppunit/extensions/HelperMacros.h>

#include <common/math/geometry/3d/object/OBBoxTest.h>
#include <AssertHelper.h>
using namespace urchin;

void OBBoxTest::noIntersection() {
    OBBox oBBox(AABBox<float>(Point3<float>(0.0f, 0.0f, 0.0f), Point3<float>(1.0f, 1.0f, 1.0f)));
    Line3D line(Point3<float>(-2.0f, 4.0f, 4.0f), Point3<float>(2.0f, 1.0f, 1.0f));

    bool hasIntersection = false;
    oBBox.intersectPoint(line, hasIntersection);

    AssertHelper::assertTrue(!hasIntersection);
}

void OBBoxTest::leftToRightLineIntersection() {
    OBBox oBBox(AABBox<float>(Point3<float>(0.0f, 0.0f, 0.0f), Point3<float>(1.0f, 1.0f, 1.0f)));
    Line3D line(Point3<float>(-2.0f, 0.5f, 0.5f), Point3<float>(2.0f, 0.5f, 0.5f));

    bool hasIntersection = false;
    Point3<float> intersectionPoint = oBBox.intersectPoint(line, hasIntersection);

    AssertHelper::assertTrue(hasIntersection);
    AssertHelper::assertPoint3FloatEquals(intersectionPoint, Point3<float>(0.0f, 0.5f, 0.5f));
}

void OBBoxTest::rightToLeftLineIntersection() {
    OBBox oBBox(AABBox<float>(Point3<float>(0.0f, 0.0f, 0.0f), Point3<float>(1.0f, 1.0f, 1.0f)));
    Line3D line(Point3<float>(2.0f, 0.5f, 0.5f), Point3<float>(-2.0f, 0.5f, 0.5f));

    bool hasIntersection = false;
    Point3<float> intersectionPoint = oBBox.intersectPoint(line, hasIntersection);

    AssertHelper::assertTrue(hasIntersection);
    AssertHelper::assertPoint3FloatEquals(intersectionPoint, Point3<float>(1.0f, 0.5f, 0.5f));
}

void OBBoxTest::farToNearLineIntersection() { //TODO review...
    OBBox oBBox(AABBox<float>(Point3<float>(0.0f, 0.0f, 0.0f), Point3<float>(1.0f, 1.0f, 1.0f)));
    Line3D line(Point3<float>(0.5f, 0.5f, -0.5f), Point3<float>(0.5f, 0.5f, 0.5f));

    bool hasIntersection = false;
    Point3<float> intersectionPoint = oBBox.intersectPoint(line, hasIntersection);

    AssertHelper::assertTrue(hasIntersection);
    AssertHelper::assertPoint3FloatEquals(intersectionPoint, Point3<float>(0.5f, 0.5f, 0.0f));
}

void OBBoxTest::bottomRightToTopLeftLineIntersection() {
    OBBox oBBox(AABBox<float>(Point3<float>(0.0f, 0.0f, 0.0f), Point3<float>(1.0f, 1.0f, 1.0f)));
    Line3D line(Point3<float>(0.5f, 0.0f, 0.5f), Point3<float>(0.0f, 0.5f, 0.5f));

    bool hasIntersection = false;
    Point3<float> intersectionPoint = oBBox.intersectPoint(line, hasIntersection);

    AssertHelper::assertTrue(hasIntersection);
    AssertHelper::assertPoint3FloatEquals(intersectionPoint, Point3<float>(0.5f, 0.0f, 0.5f));
}

void OBBoxTest::oBBoxObliqueIntersection() {
    OBBox oBBox(Vector3<float>(1.0f, 1.0f, 1.0f), Point3<float>(0.0f, 0.0f, 0.0f), Quaternion<float>::rotationZ(0.78539816339f) /* 45 degrees */);
    Line3D line(Point3<float>(0.0f, 3.0f, 0.0f), Point3<float>(0.0f, -3.0f, 0.0f));

    bool hasIntersection = false;
    Point3<float> intersectionPoint = oBBox.intersectPoint(line, hasIntersection);

    AssertHelper::assertTrue(hasIntersection);
    AssertHelper::assertPoint3FloatEquals(intersectionPoint, Point3<float>(0.0f, 1.414213f, 0.0f));
}

CppUnit::Test* OBBoxTest::suite() {
    auto* suite = new CppUnit::TestSuite("OBBoxTest");

    suite->addTest(new CppUnit::TestCaller<OBBoxTest>("noIntersection", &OBBoxTest::noIntersection));
    suite->addTest(new CppUnit::TestCaller<OBBoxTest>("leftToRightLineIntersection", &OBBoxTest::leftToRightLineIntersection));
    suite->addTest(new CppUnit::TestCaller<OBBoxTest>("rightToLeftLineIntersection", &OBBoxTest::rightToLeftLineIntersection));
    suite->addTest(new CppUnit::TestCaller<OBBoxTest>("farToNearLineIntersection", &OBBoxTest::farToNearLineIntersection));
    suite->addTest(new CppUnit::TestCaller<OBBoxTest>("bottomRightToTopLeftLineIntersection", &OBBoxTest::bottomRightToTopLeftLineIntersection));
    suite->addTest(new CppUnit::TestCaller<OBBoxTest>("oBBoxObliqueIntersection", &OBBoxTest::oBBoxObliqueIntersection));

    return suite;
}
