#include <cppunit/extensions/HelperMacros.h>
#include <UrchinCommon.h>

#include <common/math/geometry/3d/object/LineSegment3DTest.h>
#include <AssertHelper.h>
using namespace urchin;

void LineSegment3DTest::closestPoint() {
    std::array<float, 2> barycentrics{};
    LineSegment3D lineSegment(Point3<float>(1.0f, 1.0f, 0.0f), Point3<float>(2.0f, 1.0f, 0.0f));

    //point outside segment (right)
    Point3<float> closestPoint1 = lineSegment.closestPoint(Point3<float>(3.0f, 2.0f, 0.0f), barycentrics);
    AssertHelper::assertPoint3FloatEquals(closestPoint1, Point3<float>(2.0f, 1.0f, 0.0f));
    AssertHelper::assertFloatEquals(barycentrics[0], 0.0f);
    AssertHelper::assertFloatEquals(barycentrics[1], 1.0f);

    //point in segment
    Point3<float> closestPoint2 = lineSegment.closestPoint(Point3<float>(1.5f, 2.0f, 0.0f), barycentrics);
    AssertHelper::assertPoint3FloatEquals(closestPoint2, Point3<float>(1.5f, 1.0f, 0.0f));
    AssertHelper::assertFloatEquals(barycentrics[0], 0.5f);
    AssertHelper::assertFloatEquals(barycentrics[1], 0.5f);

    //point outside segment (left)
    Point3<float> closestPoint3 = lineSegment.closestPoint(Point3<float>(-3.0f, 2.0f, 0.0f), barycentrics);
    AssertHelper::assertPoint3FloatEquals(closestPoint3, Point3<float>(1.0f, 1.0f, 0.0f));
    AssertHelper::assertFloatEquals(barycentrics[0], 1.0f);
    AssertHelper::assertFloatEquals(barycentrics[1], 0.0f);
}

CppUnit::Test* LineSegment3DTest::suite() {
    auto* suite = new CppUnit::TestSuite("LineSegment3DTest");

    suite->addTest(new CppUnit::TestCaller<LineSegment3DTest>("closestPoint", &LineSegment3DTest::closestPoint));

    return suite;
}
