#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <vector>
#include <UrchinCommon.h>

#include <common/math/geometry/3d/util/SortPointsServiceTest.h>
#include <AssertHelper.h>
using namespace urchin;

void SortPointsServiceTest::threePointsClockwise() {
    std::vector threePoints({
            Point3(0.0f, 0.0f, 0.0f),
            Point3(0.5f, 0.0f, -1.0f),
            Point3(1.0f, 0.0f, 0.0f)});

    std::vector<Point3<float>> sortedPoints = SortPointsService<float>::sortPointsClockwise(threePoints, Vector3(0.0f, -1.0f, 0.0f));

    AssertHelper::assertPoint3FloatEquals(sortedPoints[0], Point3(0.0f, 0.0f, 0.0f));
    AssertHelper::assertPoint3FloatEquals(sortedPoints[1], Point3(1.0f, 0.0f, 0.0f));
    AssertHelper::assertPoint3FloatEquals(sortedPoints[2], Point3(0.5f, 0.0f, -1.0f));
}

void SortPointsServiceTest::threePointsCounterClockwise() {
    std::vector threePoints({
            Point3(0.0f, 0.0f, 0.0f),
            Point3(1.0f, 0.0f, 0.0f),
            Point3(0.5f, 0.0f, -1.0f)});

    std::vector<Point3<float>> sortedPoints = SortPointsService<float>::sortPointsCounterClockwise(threePoints, Vector3(0.0f, -1.0f, 0.0f));

    AssertHelper::assertPoint3FloatEquals(sortedPoints[0], Point3(0.0f, 0.0f, 0.0f));
    AssertHelper::assertPoint3FloatEquals(sortedPoints[1], Point3(0.5f, 0.0f, -1.0f));
    AssertHelper::assertPoint3FloatEquals(sortedPoints[2], Point3(1.0f, 0.0f, 0.0f));
}

void SortPointsServiceTest::fourPointsClockwise() {
    std::vector fourPoints({
            Point3(0.0f, 0.0f, 0.0f),
            Point3(1.0f, 0.0f, -1.0f),
            Point3(1.0f, 0.0f, 0.0f),
            Point3(0.0f, 0.0f, -1.0f)});

    std::vector<Point3<float>> sortedPoints = SortPointsService<float>::sortPointsClockwise(fourPoints, Vector3(0.0f, -1.0f, 0.0f));

    AssertHelper::assertPoint3FloatEquals(sortedPoints[0], Point3(0.0f, 0.0f, 0.0f));
    AssertHelper::assertPoint3FloatEquals(sortedPoints[1], Point3(1.0f, 0.0f, 0.0f));
    AssertHelper::assertPoint3FloatEquals(sortedPoints[2], Point3(1.0f, 0.0f, -1.0f));
    AssertHelper::assertPoint3FloatEquals(sortedPoints[3], Point3(0.0f, 0.0f, -1.0f));
}

void SortPointsServiceTest::fourPointsCounterClockwiseZNormal() {
    std::vector fourPoints({
            Point3(-2.0f, 0.0f, -5.0f),
            Point3(1.0f, 1.0f, -5.0f),
            Point3(0.0f, 2.0f, -5.0f),
            Point3(-1.0f, 1.0f, -5.0f)});

    std::vector<Point3<float>> sortedPoints = SortPointsService<float>::sortPointsCounterClockwise(fourPoints, Vector3(0.0f, 0.0f, -1.0f));

    AssertHelper::assertPoint3FloatEquals(sortedPoints[0], Point3(-2.0f, 0.0f, -5.0f));
    AssertHelper::assertPoint3FloatEquals(sortedPoints[1], Point3(-1.0f, 1.0f, -5.0f));
    AssertHelper::assertPoint3FloatEquals(sortedPoints[2], Point3(0.0f, 2.0f, -5.0f));
    AssertHelper::assertPoint3FloatEquals(sortedPoints[3], Point3(1.0f, 1.0f, -5.0f));
}

void SortPointsServiceTest::fourPointsCounterClockwiseYNormal() {
    std::vector fourPoints({
            Point3(1.0f, 0.0f, -1.0f),
            Point3(1.0f, 0.0f, 1.0f),
            Point3(-1.0f, 0.0f, -1.0f),
            Point3(-1.0f, 0.0f, 1.0f)});

    std::vector<Point3<float>> sortedPoints = SortPointsService<float>::sortPointsCounterClockwise(fourPoints, Vector3(0.0f, -1.0f, 0.0f));

    AssertHelper::assertPoint3FloatEquals(sortedPoints[0], Point3(1.0f, 0.0f, -1.0f));
    AssertHelper::assertPoint3FloatEquals(sortedPoints[1], Point3(1.0f, 0.0f, 1.0f));
    AssertHelper::assertPoint3FloatEquals(sortedPoints[2], Point3(-1.0f, 0.0f, 1.0f));
    AssertHelper::assertPoint3FloatEquals(sortedPoints[3], Point3(-1.0f, 0.0f, -1.0f));
}

CppUnit::Test* SortPointsServiceTest::suite() {
    auto* suite = new CppUnit::TestSuite("SortPointsServiceTest");

    suite->addTest(new CppUnit::TestCaller("threePointsClockwise", &SortPointsServiceTest::threePointsClockwise));
    suite->addTest(new CppUnit::TestCaller("threePointsCounterClockwise", &SortPointsServiceTest::threePointsCounterClockwise));

    suite->addTest(new CppUnit::TestCaller("fourPointsClockwise", &SortPointsServiceTest::fourPointsClockwise));
    suite->addTest(new CppUnit::TestCaller("fourPointsCounterClockwiseZNormal", &SortPointsServiceTest::fourPointsCounterClockwiseZNormal));
    suite->addTest(new CppUnit::TestCaller("fourPointsCounterClockwiseYNormal", &SortPointsServiceTest::fourPointsCounterClockwiseYNormal));

    return suite;
}

