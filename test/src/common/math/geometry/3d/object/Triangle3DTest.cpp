#include <cppunit/extensions/HelperMacros.h>
#include <UrchinCommon.h>

#include <common/math/geometry/3d/object/Triangle3DTest.h>
#include <AssertHelper.h>
using namespace urchin;

void Triangle3DTest::closestPoint() {
    std::array<float, 3> barycentrics{};
    Triangle3D triangle(Point3<float>(1.0f, 1.0f, 0.0f), Point3<float>(3.0f, 1.0f, 0.0f), Point3<float>(2.0f, 3.0f, 0.0f));

    //point in front of triangle
    Point3<float> closestPoint1 = triangle.closestPoint(Point3<float>(2.0f, 2.0f, 1.0f), barycentrics);
    AssertHelper::assertPoint3FloatEquals(closestPoint1, Point3<float>(2.0f, 2.0f, 0.0f));
    AssertHelper::assertFloatEquals(barycentrics[0], 0.25f);
    AssertHelper::assertFloatEquals(barycentrics[1], 0.25f);
    AssertHelper::assertFloatEquals(barycentrics[2], 0.5f);

    //point in voronoi region of A
    Point3<float> closestPoint2 = triangle.closestPoint(Point3<float>(0.0f, 0.0f, 1.0f), barycentrics);
    AssertHelper::assertPoint3FloatEquals(closestPoint2, Point3<float>(1.0f, 1.0f, 0.0f));
    AssertHelper::assertFloatEquals(barycentrics[0], 1.0f);
    AssertHelper::assertFloatEquals(barycentrics[1], 0.0f);
    AssertHelper::assertFloatEquals(barycentrics[2], 0.0f);

    //point in voronoi region of B
    Point3<float> closestPoint3 = triangle.closestPoint(Point3<float>(4.0f, 0.0f, 0.0f), barycentrics);
    AssertHelper::assertPoint3FloatEquals(closestPoint3, Point3<float>(3.0f, 1.0f, 0.0f));
    AssertHelper::assertFloatEquals(barycentrics[0], 0.0f);
    AssertHelper::assertFloatEquals(barycentrics[1], 1.0f);
    AssertHelper::assertFloatEquals(barycentrics[2], 0.0f);

    //point in voronoi region of C
    Point3<float> closestPoint4 = triangle.closestPoint(Point3<float>(2.1f, 4.0f, 1.0f), barycentrics);
    AssertHelper::assertPoint3FloatEquals(closestPoint4, Point3<float>(2.0f, 3.0f, 0.0f));
    AssertHelper::assertFloatEquals(barycentrics[0], 0.0f);
    AssertHelper::assertFloatEquals(barycentrics[1], 0.0f);
    AssertHelper::assertFloatEquals(barycentrics[2], 1.0f);

    //point in voronoi region of AB
    Point3<float> closestPoint5 = triangle.closestPoint(Point3<float>(2.0f, 0.0f, 5.0f), barycentrics);
    AssertHelper::assertPoint3FloatEquals(closestPoint5, Point3<float>(2.0f, 1.0f, 0.0f));
    AssertHelper::assertFloatEquals(barycentrics[0], 0.5f);
    AssertHelper::assertFloatEquals(barycentrics[1], 0.5f);
    AssertHelper::assertFloatEquals(barycentrics[2], 0.0f);

    //point in voronoi region of AC
    Point3<float> closestPoint6 = triangle.closestPoint(Point3<float>(0.0f, 2.0f, 1.0f), barycentrics);
    AssertHelper::assertPoint3FloatEquals(closestPoint6, Point3<float>(1.2f, 1.4f, 0.0f));
    AssertHelper::assertFloatEquals(barycentrics[0], 0.8f);
    AssertHelper::assertFloatEquals(barycentrics[1], 0.0f);
    AssertHelper::assertFloatEquals(barycentrics[2], 0.2f);

    //point in voronoi region of BC
    Point3<float> closestPoint7 = triangle.closestPoint(Point3<float>(3.1f, 2.0f, 5.0f), barycentrics);
    AssertHelper::assertPoint3FloatEquals(closestPoint7, Point3<float>(2.62f, 1.76f, 0.0f));
    AssertHelper::assertFloatEquals(barycentrics[0], 0.0f);
    AssertHelper::assertFloatEquals(barycentrics[1], 0.62f);
    AssertHelper::assertFloatEquals(barycentrics[2], 0.38f);
}

CppUnit::Test* Triangle3DTest::suite() {
    auto* suite = new CppUnit::TestSuite("Triangle3DTest");

    suite->addTest(new CppUnit::TestCaller<Triangle3DTest>("closestPoint", &Triangle3DTest::closestPoint));

    return suite;
}
