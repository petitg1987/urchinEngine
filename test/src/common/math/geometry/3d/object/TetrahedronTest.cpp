#include <cppunit/extensions/HelperMacros.h>
#include <UrchinCommon.h>

#include <common/math/geometry/3d/object/TetrahedronTest.h>
#include <AssertHelper.h>
using namespace urchin;

void TetrahedronTest::closestPoint() {
    std::array<float, 4> barycentrics{};
    Tetrahedron tetrahedron(Point3(1.0f, 0.0f, 1.0f), Point3(2.0f, 0.0f, 0.0f), Point3(3.0f, 0.0f, 1.0f), Point3(2.0f, 3.0f, 0.5f));

    //point in voronoi region of D
    Point3<float> closestPoint1 = tetrahedron.closestPoint(Point3(2.0f, 3.0f, 0.5f), barycentrics);
    AssertHelper::assertPoint3FloatEquals(closestPoint1, Point3(2.0f, 3.0f, 0.5f));
    AssertHelper::assertFloatEquals(barycentrics[0], 0.0f);
    AssertHelper::assertFloatEquals(barycentrics[1], 0.0f);
    AssertHelper::assertFloatEquals(barycentrics[2], 0.0f);
    AssertHelper::assertFloatEquals(barycentrics[3], 1.0f);

    //point in voronoi region of C
    Point3<float> closestPoint2 = tetrahedron.closestPoint(Point3(4.0f, 0.0f, 1.0f), barycentrics);
    AssertHelper::assertPoint3FloatEquals(closestPoint2, Point3(3.0f, 0.0f, 1.0f));
    AssertHelper::assertFloatEquals(barycentrics[0], 0.0f);
    AssertHelper::assertFloatEquals(barycentrics[1], 0.0f);
    AssertHelper::assertFloatEquals(barycentrics[2], 1.0f);
    AssertHelper::assertFloatEquals(barycentrics[3], 0.0f);

    //point in voronoi region of BD
    Point3<float> closestPoint3 = tetrahedron.closestPoint(Point3(2.0f, 2.0f, -1.0f), barycentrics);
    AssertHelper::assertPoint3FloatEquals(closestPoint3, Point3(2.0f, 1.78378379f, 0.29729723f));
    AssertHelper::assertFloatEquals(barycentrics[0], 0.0f);
    AssertHelper::assertFloatEquals(barycentrics[1], 0.405405f);
    AssertHelper::assertFloatEquals(barycentrics[2], 0.0f);
    AssertHelper::assertFloatEquals(barycentrics[3], 0.594595f);

    //point in voronoi region of ABC
    Point3<float> closestPoint4 = tetrahedron.closestPoint(Point3(2.0f, -5.0f, 0.5f), barycentrics);
    AssertHelper::assertPoint3FloatEquals(closestPoint4, Point3(2.0f, 0.0f, 0.5f));
    AssertHelper::assertFloatEquals(barycentrics[0], 0.25f);
    AssertHelper::assertFloatEquals(barycentrics[1], 0.5f);
    AssertHelper::assertFloatEquals(barycentrics[2], 0.25f);
    AssertHelper::assertFloatEquals(barycentrics[3], 0.0f);

    //point in voronoi region of ABCD (near B)
    Point3<float> closestPoint5 = tetrahedron.closestPoint(Point3(1.9f, 0.5f, 0.5f), barycentrics);
    AssertHelper::assertPoint3FloatEquals(closestPoint5, Point3(1.9f, 0.5f, 0.5f));
    AssertHelper::assertFloatEquals(barycentrics[0], 0.258333f);
    AssertHelper::assertFloatEquals(barycentrics[1], 0.416667f);
    AssertHelper::assertFloatEquals(barycentrics[2], 0.158333f);
    AssertHelper::assertFloatEquals(barycentrics[3], 0.166667f);

    //point in voronoi region of ABCD (near D)
    Point3<float> closestPoint6 = tetrahedron.closestPoint(Point3(2.0f, 2.9f, 0.5f), barycentrics);
    AssertHelper::assertPoint3FloatEquals(closestPoint6, Point3(2.0f, 2.9f, 0.5f));
    AssertHelper::assertFloatEquals(barycentrics[0], 0.008333f);
    AssertHelper::assertFloatEquals(barycentrics[1], 0.016667f);
    AssertHelper::assertFloatEquals(barycentrics[2], 0.008333f);
    AssertHelper::assertFloatEquals(barycentrics[3], 0.966667f);
}

CppUnit::Test* TetrahedronTest::suite() {
    auto* suite = new CppUnit::TestSuite("TetrahedronTest");

    suite->addTest(new CppUnit::TestCaller<TetrahedronTest>("closestPoint", &TetrahedronTest::closestPoint));

    return suite;
}
