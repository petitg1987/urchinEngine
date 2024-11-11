#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <vector>
#include <memory>
#include <UrchinCommon.h>

#include <common/math/geometry/3d/object/ConvexHull3DTest.h>
#include <AssertHelper.h>
using namespace urchin;

void ConvexHull3DTest::reduceBox() {
    std::vector<Point3<float>> aabbPoints = {
            Point3(0.0f, 1.0f, 0.0f), Point3(1.0f, 1.0f, 0.0f), Point3(1.0f, -1.0f, 0.0f), Point3(0.0f, -1.0f, 0.0f),
            Point3(0.0f, 1.0f, -1.0f), Point3(1.0f, 1.0f, -1.0f), Point3(1.0f, -1.0f, -1.0f), Point3(0.0f, -1.0f, -1.0f)};
    ConvexHull3D<float> convexHull(aabbPoints);

    std::unique_ptr<ConvexHull3D<float>> convexHullResult = convexHull.resize(-0.1f);

    AssertHelper::assertPoint3FloatEquals(convexHullResult->getSupportPoint(Vector3(1.0f, 1.0f, -1.0f)), Point3(0.9f, 0.9f, -0.9f));
    AssertHelper::assertPoint3FloatEquals(convexHullResult->getSupportPoint(Vector3(1.0f, 1.0f, 1.0f)), Point3(0.9f, 0.9f, -0.1f));
    AssertHelper::assertPoint3FloatEquals(convexHullResult->getSupportPoint(Vector3(-1.0f, 1.0f, -1.0f)), Point3(0.1f, 0.9f, -0.9f));
    AssertHelper::assertPoint3FloatEquals(convexHullResult->getSupportPoint(Vector3(-1.0f, 1.0f, 1.0f)), Point3(0.1f, 0.9f, -0.1f));
    AssertHelper::assertPoint3FloatEquals(convexHullResult->getSupportPoint(Vector3(1.0f, -1.0f, -1.0f)), Point3(0.9f, -0.9f, -0.9f));
    AssertHelper::assertPoint3FloatEquals(convexHullResult->getSupportPoint(Vector3(1.0f, -1.0f, 1.0f)), Point3(0.9f, -0.9f, -0.1f));
    AssertHelper::assertPoint3FloatEquals(convexHullResult->getSupportPoint(Vector3(-1.0f, -1.0f, -1.0f)), Point3(0.1f, -0.9f, -0.9f));
    AssertHelper::assertPoint3FloatEquals(convexHullResult->getSupportPoint(Vector3(-1.0f, -1.0f, 1.0f)), Point3(0.1f, -0.9f, -0.1f));
}

void ConvexHull3DTest::nullExpandTetrahedron() {
    std::vector<Point3<float>> tetrahedronPoints = {
                Point3(-1.0f, 0.0f, 0.0f), Point3(0.0f, 0.0f, 1.0f), Point3(1.0f, 0.0f, 0.0f), Point3(0.0f, 3.0f, 0.0f)};
    ConvexHull3D<float> convexHull(tetrahedronPoints);

    std::unique_ptr<ConvexHull3D<float>> convexHullResult = convexHull.resize(0.0f);

    AssertHelper::assertPoint3FloatEquals(convexHullResult->getSupportPoint(Vector3(-1.0f, -1.0f, -1.0f)), tetrahedronPoints[0]); //left point
    AssertHelper::assertPoint3FloatEquals(convexHullResult->getSupportPoint(Vector3(0.0f, -1.0f, 1.0f)), tetrahedronPoints[1]); //middle bottom point
    AssertHelper::assertPoint3FloatEquals(convexHullResult->getSupportPoint(Vector3(1.0f, -1.0f, -1.0f)), tetrahedronPoints[2]); //right point
    AssertHelper::assertPoint3FloatEquals(convexHullResult->getSupportPoint(Vector3(0.0f, 1.0f, 0.0f)), tetrahedronPoints[3]); //top point
}

void ConvexHull3DTest::expandTetrahedron() {
    std::vector<Point3<float>> tetrahedronPoints = {
                Point3(-1.0f, 0.0f, 0.0f), Point3(0.0f, 0.0f, 1.0f), Point3(1.0f, 0.0f, 0.0f), Point3(0.0f, 3.0f, 0.0f)};
    ConvexHull3D<float> convexHull(tetrahedronPoints);

    std::unique_ptr<ConvexHull3D<float>> convexHullResult = convexHull.resize(0.5f);

    AssertHelper::assertPoint3FloatEquals(convexHullResult->getSupportPoint(Vector3(-1.0f, -1.0f, -1.0f)), Point3(-2.39315f, -0.5f, -0.5f)); //left point
    AssertHelper::assertPoint3FloatEquals(convexHullResult->getSupportPoint(Vector3(0.0f, -1.0f, 1.0f)), Point3(0.0f, -0.5f, 1.89315f)); //middle bottom point
    AssertHelper::assertPoint3FloatEquals(convexHullResult->getSupportPoint(Vector3(1.0f, -1.0f, -1.0f)), Point3(2.39315f, -0.5f, -0.5f)); //right point
    AssertHelper::assertPoint3FloatEquals(convexHullResult->getSupportPoint(Vector3(0.0f, 1.0f, 0.0f)), Point3(0.0f, 6.67945f, -0.5f)); //top point
}

CppUnit::Test* ConvexHull3DTest::suite() {
    auto* suite = new CppUnit::TestSuite("ConvexHull3DTest");

    suite->addTest(new CppUnit::TestCaller("reduceBox", &ConvexHull3DTest::reduceBox));

    suite->addTest(new CppUnit::TestCaller("nullExpandTetrahedron", &ConvexHull3DTest::nullExpandTetrahedron));
    suite->addTest(new CppUnit::TestCaller("expandTetrahedron", &ConvexHull3DTest::expandTetrahedron));

    return suite;
}
