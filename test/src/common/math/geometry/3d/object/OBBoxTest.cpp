#include <cppunit/extensions/HelperMacros.h>

#include <common/math/geometry/3d/object/OBBoxTest.h>
#include <AssertHelper.h>
using namespace urchin;

void OBBoxTest::leftRightLineIntersection() {
    OBBox<float> oBBox(AABBox<float>(Point3<float>(0.0f, 0.0f, 0.0f), Point3<float>(1.0f, 1.0f, 1.0f)));
    Line3D<float> line(Point3<float>(-2.0f, 0.5f, 0.5f), Point3<float>(2.0f, 0.5f, 0.5f));

    bool hasIntersection = false;
    Point3<float> intersectionPoint = oBBox.intersectPoint(line, hasIntersection);

    AssertHelper::assertTrue(hasIntersection);
    AssertHelper::assertPoint3FloatEquals(intersectionPoint, Point3<float>(0.0f, 0.5f, 0.5f));
}

void OBBoxTest::rightLeftLineIntersection() {
    OBBox<float> oBBox(AABBox<float>(Point3<float>(0.0f, 0.0f, 0.0f), Point3<float>(1.0f, 1.0f, 1.0f)));
    Line3D<float> line(Point3<float>(2.0f, 0.5f, 0.5f), Point3<float>(-2.0f, 0.5f, 0.5f));

    bool hasIntersection = false;
    Point3<float> intersectionPoint = oBBox.intersectPoint(line, hasIntersection);

    AssertHelper::assertTrue(hasIntersection);
    AssertHelper::assertPoint3FloatEquals(intersectionPoint, Point3<float>(1.0f, 0.5f, 0.5f));
}

//TODO add more test

CppUnit::Test* OBBoxTest::suite() {
    auto* suite = new CppUnit::TestSuite("OBBoxTest");

    suite->addTest(new CppUnit::TestCaller<OBBoxTest>("leftRightLineIntersection", &OBBoxTest::leftRightLineIntersection));
    suite->addTest(new CppUnit::TestCaller<OBBoxTest>("rightLeftLineIntersection", &OBBoxTest::rightLeftLineIntersection));

    return suite;
}
