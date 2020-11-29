#include <cppunit/extensions/HelperMacros.h>
#include "UrchinCommon.h"

#include "common/math/geometry/ClosestPointTest.h"
#include "AssertHelper.h"
using namespace urchin;

void ClosestPointTest::closestPointLineSegment2D() {
    float barycentrics[2];
    LineSegment2D<float> lineSegment(Point2<float>(1.0, 1.0), Point2<float>(2.0, 1.0));

    //point outside segment (right)
    Point2<float> closestPoint1 = lineSegment.closestPoint(Point2<float>(3.0, 2.0), barycentrics);
    AssertHelper::assertPoint2FloatEquals(closestPoint1, Point2<float>(2.0, 1.0));
    AssertHelper::assertFloatEquals(barycentrics[0], 0.0);
    AssertHelper::assertFloatEquals(barycentrics[1], 1.0);

    //point in segment
    Point2<float> closestPoint2 = lineSegment.closestPoint(Point2<float>(1.5, 2.0), barycentrics);
    AssertHelper::assertPoint2FloatEquals(closestPoint2, Point2<float>(1.5, 1.0));
    AssertHelper::assertFloatEquals(barycentrics[0], 0.5);
    AssertHelper::assertFloatEquals(barycentrics[1], 0.5);

    //point outside segment (left)
    Point2<float> closestPoint3 = lineSegment.closestPoint(Point2<float>(-3.0, 2.0),barycentrics);
    AssertHelper::assertPoint2FloatEquals(closestPoint3, Point2<float>(1.0, 1.0));
    AssertHelper::assertFloatEquals(barycentrics[0], 1.0);
    AssertHelper::assertFloatEquals(barycentrics[1], 0.0);
}

void ClosestPointTest::closestPointLineSegment3D() {
    float barycentrics[2];
    LineSegment3D<float> lineSegment(Point3<float>(1.0, 1.0, 0.0), Point3<float>(2.0, 1.0, 0.0));

    //point outside segment (right)
    Point3<float> closestPoint1 = lineSegment.closestPoint(Point3<float>(3.0, 2.0, 0.0), barycentrics);
    AssertHelper::assertPoint3FloatEquals(closestPoint1, Point3<float>(2.0, 1.0, 0.0));
    AssertHelper::assertFloatEquals(barycentrics[0], 0.0);
    AssertHelper::assertFloatEquals(barycentrics[1], 1.0);

    //point in segment
    Point3<float> closestPoint2 = lineSegment.closestPoint(Point3<float>(1.5, 2.0, 0.0), barycentrics);
    AssertHelper::assertPoint3FloatEquals(closestPoint2, Point3<float>(1.5, 1.0, 0.0));
    AssertHelper::assertFloatEquals(barycentrics[0], 0.5);
    AssertHelper::assertFloatEquals(barycentrics[1], 0.5);

    //point outside segment (left)
    Point3<float> closestPoint3 = lineSegment.closestPoint(Point3<float>(-3.0, 2.0, 0.0), barycentrics);
    AssertHelper::assertPoint3FloatEquals(closestPoint3, Point3<float>(1.0, 1.0, 0.0));
    AssertHelper::assertFloatEquals(barycentrics[0], 1.0);
    AssertHelper::assertFloatEquals(barycentrics[1], 0.0);
}

void ClosestPointTest::closestPointTriangle3D() {
    float barycentrics[3];
    Triangle3D<float> triangle(Point3<float>(1.0, 1.0, 0.0), Point3<float>(3.0, 1.0, 0.0), Point3<float>(2.0, 3.0, 0.0));

    //point in front of triangle
    Point3<float> closestPoint1 = triangle.closestPoint(Point3<float>(2.0, 2.0, 1.0), barycentrics);
    AssertHelper::assertPoint3FloatEquals(closestPoint1, Point3<float>(2.0, 2.0, 0.0));
    AssertHelper::assertFloatEquals(barycentrics[0], 0.25);
    AssertHelper::assertFloatEquals(barycentrics[1], 0.25);
    AssertHelper::assertFloatEquals(barycentrics[2], 0.5);

    //point in voronoi region of A
    Point3<float> closestPoint2 = triangle.closestPoint(Point3<float>(0.0, 0.0, 1.0), barycentrics);
    AssertHelper::assertPoint3FloatEquals(closestPoint2, Point3<float>(1.0, 1.0, 0.0));
    AssertHelper::assertFloatEquals(barycentrics[0], 1.0);
    AssertHelper::assertFloatEquals(barycentrics[1], 0.0);
    AssertHelper::assertFloatEquals(barycentrics[2], 0.0);

    //point in voronoi region of B
    Point3<float> closestPoint3 = triangle.closestPoint(Point3<float>(4.0, 0.0, 0.0), barycentrics);
    AssertHelper::assertPoint3FloatEquals(closestPoint3, Point3<float>(3.0, 1.0, 0.0));
    AssertHelper::assertFloatEquals(barycentrics[0], 0.0);
    AssertHelper::assertFloatEquals(barycentrics[1], 1.0);
    AssertHelper::assertFloatEquals(barycentrics[2], 0.0);

    //point in voronoi region of C
    Point3<float> closestPoint4 = triangle.closestPoint(Point3<float>(2.1, 4.0, 1.0), barycentrics);
    AssertHelper::assertPoint3FloatEquals(closestPoint4, Point3<float>(2.0, 3.0, 0.0));
    AssertHelper::assertFloatEquals(barycentrics[0], 0.0);
    AssertHelper::assertFloatEquals(barycentrics[1], 0.0);
    AssertHelper::assertFloatEquals(barycentrics[2], 1.0);

    //point in voronoi region of AB
    Point3<float> closestPoint5 = triangle.closestPoint(Point3<float>(2.0, 0.0, 5.0), barycentrics);
    AssertHelper::assertPoint3FloatEquals(closestPoint5, Point3<float>(2.0, 1.0, 0.0));
    AssertHelper::assertFloatEquals(barycentrics[0], 0.5);
    AssertHelper::assertFloatEquals(barycentrics[1], 0.5);
    AssertHelper::assertFloatEquals(barycentrics[2], 0.0);

    //point in voronoi region of AC
    Point3<float> closestPoint6 = triangle.closestPoint(Point3<float>(0.0, 2.0, 1.0), barycentrics);
    AssertHelper::assertPoint3FloatEquals(closestPoint6, Point3<float>(1.2, 1.4, 0.0));
    AssertHelper::assertFloatEquals(barycentrics[0], 0.8);
    AssertHelper::assertFloatEquals(barycentrics[1], 0.0);
    AssertHelper::assertFloatEquals(barycentrics[2], 0.2);

    //point in voronoi region of BC
    Point3<float> closestPoint7 = triangle.closestPoint(Point3<float>(3.1, 2.0, 5.0), barycentrics);
    AssertHelper::assertPoint3FloatEquals(closestPoint7, Point3<float>(2.62, 1.76, 0.0));
    AssertHelper::assertFloatEquals(barycentrics[0], 0.0);
    AssertHelper::assertFloatEquals(barycentrics[1], 0.62);
    AssertHelper::assertFloatEquals(barycentrics[2], 0.38);
}

void ClosestPointTest::closestPointTetrahedron() {
    float barycentrics[4];
    Tetrahedron<float> tetrahedron(Point3<float>(1.0, 0.0, 1.0), Point3<float>(2.0, 0.0, 0.0), Point3<float>(3.0, 0.0, 1.0), Point3<float>(2.0, 3.0, 0.5));

    //point in voronoi region of D
    Point3<float> closestPoint1 = tetrahedron.closestPoint(Point3<float>(2.0, 3.0, 0.5), barycentrics);
    AssertHelper::assertPoint3FloatEquals(closestPoint1, Point3<float>(2.0, 3.0, 0.5));
    AssertHelper::assertFloatEquals(barycentrics[0], 0.0);
    AssertHelper::assertFloatEquals(barycentrics[1], 0.0);
    AssertHelper::assertFloatEquals(barycentrics[2], 0.0);
    AssertHelper::assertFloatEquals(barycentrics[3], 1.0);

    //point in voronoi region of C
    Point3<float> closestPoint2 = tetrahedron.closestPoint(Point3<float>(4.0, 0.0, 1.0), barycentrics);
    AssertHelper::assertPoint3FloatEquals(closestPoint2, Point3<float>(3.0, 0.0, 1.0));
    AssertHelper::assertFloatEquals(barycentrics[0], 0.0);
    AssertHelper::assertFloatEquals(barycentrics[1], 0.0);
    AssertHelper::assertFloatEquals(barycentrics[2], 1.0);
    AssertHelper::assertFloatEquals(barycentrics[3], 0.0);

    //point in voronoi region of BD
    Point3<float> closestPoint3 = tetrahedron.closestPoint(Point3<float>(2.0, 2.0, -1.0), barycentrics);
    AssertHelper::assertPoint3FloatEquals(closestPoint3, Point3<float>(2.0, 1.78378379, 0.29729723));
    AssertHelper::assertFloatEquals(barycentrics[0], 0.0);
    AssertHelper::assertFloatEquals(barycentrics[1], 0.405405);
    AssertHelper::assertFloatEquals(barycentrics[2], 0.0);
    AssertHelper::assertFloatEquals(barycentrics[3], 0.594595);

    //point in voronoi region of ABC
    Point3<float> closestPoint4 = tetrahedron.closestPoint(Point3<float>(2.0, -5.0, 0.5), barycentrics);
    AssertHelper::assertPoint3FloatEquals(closestPoint4, Point3<float>(2.0, 0.0, 0.5));
    AssertHelper::assertFloatEquals(barycentrics[0], 0.25);
    AssertHelper::assertFloatEquals(barycentrics[1], 0.5);
    AssertHelper::assertFloatEquals(barycentrics[2], 0.25);
    AssertHelper::assertFloatEquals(barycentrics[3], 0.0);

    //point in voronoi region of ABCD (near B)
    Point3<float> closestPoint5 = tetrahedron.closestPoint(Point3<float>(1.9, 0.5, 0.5), barycentrics);
    AssertHelper::assertPoint3FloatEquals(closestPoint5, Point3<float>(1.9, 0.5, 0.5));
    AssertHelper::assertFloatEquals(barycentrics[0], 0.258333);
    AssertHelper::assertFloatEquals(barycentrics[1], 0.416667);
    AssertHelper::assertFloatEquals(barycentrics[2], 0.158333);
    AssertHelper::assertFloatEquals(barycentrics[3], 0.166667);

    //point in voronoi region of ABCD (near D)
    Point3<float> closestPoint6 = tetrahedron.closestPoint(Point3<float>(2.0, 2.9, 0.5), barycentrics);
    AssertHelper::assertPoint3FloatEquals(closestPoint6, Point3<float>(2.0, 2.9, 0.5));
    AssertHelper::assertFloatEquals(barycentrics[0], 0.008333);
    AssertHelper::assertFloatEquals(barycentrics[1], 0.016667);
    AssertHelper::assertFloatEquals(barycentrics[2], 0.008333);
    AssertHelper::assertFloatEquals(barycentrics[3], 0.966667);
}

CppUnit::Test* ClosestPointTest::suite() {
    auto* suite = new CppUnit::TestSuite("VoronoiRegionTest");

    suite->addTest(new CppUnit::TestCaller<ClosestPointTest>("closestPointLineSegment2D", &ClosestPointTest::closestPointLineSegment2D));
    suite->addTest(new CppUnit::TestCaller<ClosestPointTest>("closestPointLineSegment3D", &ClosestPointTest::closestPointLineSegment3D));

    suite->addTest(new CppUnit::TestCaller<ClosestPointTest>("closestPointTriangle3D", &ClosestPointTest::closestPointTriangle3D));

    suite->addTest(new CppUnit::TestCaller<ClosestPointTest>("closestPointTetrahedron", &ClosestPointTest::closestPointTetrahedron));

    return suite;
}
