#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <vector>
#include <memory>
#include "UrchinCommon.h"

#include "common/math/geometry/ResizeConvexHull3DTest.h"
#include "AssertHelper.h"
using namespace urchin;

void ResizeConvexHull3DTest::reduceBox() {
    std::vector<Point3<float>> aabbPoints = {
            Point3<float>(0.0, 1.0, 0.0), Point3<float>(1.0, 1.0, 0.0), Point3<float>(1.0, -1.0, 0.0), Point3<float>(0.0, -1.0, 0.0),
            Point3<float>(0.0, 1.0, -1.0), Point3<float>(1.0, 1.0, -1.0), Point3<float>(1.0, -1.0, -1.0), Point3<float>(0.0, -1.0, -1.0)};
    ConvexHull3D<float> convexHull(aabbPoints);

    std::unique_ptr<ConvexHull3D<float>> convexHullResult = convexHull.resize(-0.1f);

    AssertHelper::assertPoint3FloatEquals(convexHullResult->getSupportPoint(Vector3<float>(1.0, 1.0, -1.0)), Point3<float>(0.9, 0.9, -0.9));
    AssertHelper::assertPoint3FloatEquals(convexHullResult->getSupportPoint(Vector3<float>(1.0, 1.0, 1.0)), Point3<float>(0.9, 0.9, -0.1));
    AssertHelper::assertPoint3FloatEquals(convexHullResult->getSupportPoint(Vector3<float>(-1.0, 1.0, -1.0)), Point3<float>(0.1, 0.9, -0.9));
    AssertHelper::assertPoint3FloatEquals(convexHullResult->getSupportPoint(Vector3<float>(-1.0, 1.0, 1.0)), Point3<float>(0.1, 0.9, -0.1));
    AssertHelper::assertPoint3FloatEquals(convexHullResult->getSupportPoint(Vector3<float>(1.0, -1.0, -1.0)), Point3<float>(0.9, -0.9, -0.9));
    AssertHelper::assertPoint3FloatEquals(convexHullResult->getSupportPoint(Vector3<float>(1.0, -1.0, 1.0)), Point3<float>(0.9, -0.9, -0.1));
    AssertHelper::assertPoint3FloatEquals(convexHullResult->getSupportPoint(Vector3<float>(-1.0, -1.0, -1.0)), Point3<float>(0.1, -0.9, -0.9));
    AssertHelper::assertPoint3FloatEquals(convexHullResult->getSupportPoint(Vector3<float>(-1.0, -1.0, 1.0)), Point3<float>(0.1, -0.9, -0.1));
}

void ResizeConvexHull3DTest::nullExpandTetrahedron() {
    std::vector<Point3<float>> tetrahedronPoints = {
                Point3<float>(-1.0, 0.0, 0.0), Point3<float>(0.0, 0.0, 1.0), Point3<float>(1.0, 0.0, 0.0), Point3<float>(0.0, 3.0, 0.0)};
    ConvexHull3D<float> convexHull(tetrahedronPoints);

    std::unique_ptr<ConvexHull3D<float>> convexHullResult = convexHull.resize(0.0f);

    AssertHelper::assertPoint3FloatEquals(convexHullResult->getSupportPoint(Vector3<float>(-1.0, -1.0, -1.0)), tetrahedronPoints[0]); //left point
    AssertHelper::assertPoint3FloatEquals(convexHullResult->getSupportPoint(Vector3<float>(0.0, -1.0, 1.0)), tetrahedronPoints[1]); //middle bottom point
    AssertHelper::assertPoint3FloatEquals(convexHullResult->getSupportPoint(Vector3<float>(1.0, -1.0, -1.0)), tetrahedronPoints[2]); //right point
    AssertHelper::assertPoint3FloatEquals(convexHullResult->getSupportPoint(Vector3<float>(0.0, 1.0, 0.0)), tetrahedronPoints[3]); //top point
}

void ResizeConvexHull3DTest::expandTetrahedron() {
    std::vector<Point3<float>> tetrahedronPoints = {
                Point3<float>(-1.0, 0.0, 0.0), Point3<float>(0.0, 0.0, 1.0), Point3<float>(1.0, 0.0, 0.0), Point3<float>(0.0, 3.0, 0.0)};
    ConvexHull3D<float> convexHull(tetrahedronPoints);

    std::unique_ptr<ConvexHull3D<float>> convexHullResult = convexHull.resize(0.5f);

    AssertHelper::assertPoint3FloatEquals(convexHullResult->getSupportPoint(Vector3<float>(-1.0, -1.0, -1.0)), Point3<float>(-2.39315, -0.5, -0.5)); //left point
    AssertHelper::assertPoint3FloatEquals(convexHullResult->getSupportPoint(Vector3<float>(0.0, -1.0, 1.0)), Point3<float>(0.0, -0.5, 1.89315)); //middle bottom point
    AssertHelper::assertPoint3FloatEquals(convexHullResult->getSupportPoint(Vector3<float>(1.0, -1.0, -1.0)), Point3<float>(2.39315, -0.5, -0.5)); //right point
    AssertHelper::assertPoint3FloatEquals(convexHullResult->getSupportPoint(Vector3<float>(0.0, 1.0, 0.0)), Point3<float>(0.0, 6.67945, -0.5)); //top point
}

CppUnit::Test *ResizeConvexHull3DTest::suite() {
    auto* suite = new CppUnit::TestSuite("ResizeConvexHullTest");

    suite->addTest(new CppUnit::TestCaller<ResizeConvexHull3DTest>("reduceBox", &ResizeConvexHull3DTest::reduceBox));

    suite->addTest(new CppUnit::TestCaller<ResizeConvexHull3DTest>("nullExpandTetrahedron", &ResizeConvexHull3DTest::nullExpandTetrahedron));
    suite->addTest(new CppUnit::TestCaller<ResizeConvexHull3DTest>("expandTetrahedron", &ResizeConvexHull3DTest::expandTetrahedron));

    return suite;
}
