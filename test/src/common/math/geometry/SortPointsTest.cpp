#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <vector>
#include <UrchinCommon.h>

#include <common/math/geometry/SortPointsTest.h>
#include <AssertHelper.h>
using namespace urchin;

void SortPointsTest::threePointsClockwise() {
    std::vector<Point3<float>> threePoints({
            Point3<float>(0.0, 0.0, 0.0),
            Point3<float>(0.5, 0.0, -1.0),
            Point3<float>(1.0, 0.0, 0.0)});

    std::vector<Point3<float>> sortedPoints = SortPointsService<float>::sortPointsClockwise(threePoints, Vector3<float>(0.0, -1.0, 0.0));

    AssertHelper::assertPoint3FloatEquals(sortedPoints[0], Point3<float>(0.0, 0.0, 0.0));
    AssertHelper::assertPoint3FloatEquals(sortedPoints[1], Point3<float>(1.0, 0.0, 0.0));
    AssertHelper::assertPoint3FloatEquals(sortedPoints[2], Point3<float>(0.5, 0.0, -1.0));
}

void SortPointsTest::threePointsCounterClockwise() {
    std::vector<Point3<float>> threePoints({
            Point3<float>(0.0, 0.0, 0.0),
            Point3<float>(1.0, 0.0, 0.0),
            Point3<float>(0.5, 0.0, -1.0)});

    std::vector<Point3<float>> sortedPoints = SortPointsService<float>::sortPointsCounterClockwise(threePoints, Vector3<float>(0.0, -1.0, 0.0));

    AssertHelper::assertPoint3FloatEquals(sortedPoints[0], Point3<float>(0.0, 0.0, 0.0));
    AssertHelper::assertPoint3FloatEquals(sortedPoints[1], Point3<float>(0.5, 0.0, -1.0));
    AssertHelper::assertPoint3FloatEquals(sortedPoints[2], Point3<float>(1.0, 0.0, 0.0));
}

void SortPointsTest::fourPointsClockwise() {
    std::vector<Point3<float>> fourPoints({
            Point3<float>(0.0, 0.0, 0.0),
            Point3<float>(1.0, 0.0, -1.0),
            Point3<float>(1.0, 0.0, 0.0),
            Point3<float>(0.0, 0.0, -1.0)});

    std::vector<Point3<float>> sortedPoints = SortPointsService<float>::sortPointsClockwise(fourPoints, Vector3<float>(0.0, -1.0, 0.0));

    AssertHelper::assertPoint3FloatEquals(sortedPoints[0], Point3<float>(0.0, 0.0, 0.0));
    AssertHelper::assertPoint3FloatEquals(sortedPoints[1], Point3<float>(1.0, 0.0, 0.0));
    AssertHelper::assertPoint3FloatEquals(sortedPoints[2], Point3<float>(1.0, 0.0, -1.0));
    AssertHelper::assertPoint3FloatEquals(sortedPoints[3], Point3<float>(0.0, 0.0, -1.0));
}

void SortPointsTest::fourPointsCounterClockwiseZNormal() {
    std::vector<Point3<float>> fourPoints({
            Point3<float>(-2.0, 0.0, -5.0),
            Point3<float>(1.0, 1.0, -5.0),
            Point3<float>(0.0, 2.0, -5.0),
            Point3<float>(-1.0, 1.0, -5.0)});

    std::vector<Point3<float>> sortedPoints = SortPointsService<float>::sortPointsCounterClockwise(fourPoints, Vector3<float>(0.0, 0.0, -1.0));

    AssertHelper::assertPoint3FloatEquals(sortedPoints[0], Point3<float>(-2.0, 0.0, -5.0));
    AssertHelper::assertPoint3FloatEquals(sortedPoints[1], Point3<float>(-1.0, 1.0, -5.0));
    AssertHelper::assertPoint3FloatEquals(sortedPoints[2], Point3<float>(0.0, 2.0, -5.0));
    AssertHelper::assertPoint3FloatEquals(sortedPoints[3], Point3<float>(1.0, 1.0, -5.0));
}

void SortPointsTest::fourPointsCounterClockwiseYNormal() {
    std::vector<Point3<float>> fourPoints({
            Point3<float>(1.0, 0.0, -1.0),
            Point3<float>(1.0, 0.0, 1.0),
            Point3<float>(-1.0, 0.0, -1.0),
            Point3<float>(-1.0, 0.0, 1.0)});

    std::vector<Point3<float>> sortedPoints = SortPointsService<float>::sortPointsCounterClockwise(fourPoints, Vector3<float>(0.0, -1.0, 0.0));

    AssertHelper::assertPoint3FloatEquals(sortedPoints[0], Point3<float>(1.0, 0.0, -1.0));
    AssertHelper::assertPoint3FloatEquals(sortedPoints[1], Point3<float>(1.0, 0.0, 1.0));
    AssertHelper::assertPoint3FloatEquals(sortedPoints[2], Point3<float>(-1.0, 0.0, 1.0));
    AssertHelper::assertPoint3FloatEquals(sortedPoints[3], Point3<float>(-1.0, 0.0, -1.0));
}

CppUnit::Test* SortPointsTest::suite() {
    auto* suite = new CppUnit::TestSuite("SortPointsTest");

    suite->addTest(new CppUnit::TestCaller<SortPointsTest>("threePointsClockwise", &SortPointsTest::threePointsClockwise));
    suite->addTest(new CppUnit::TestCaller<SortPointsTest>("threePointsCounterClockwise", &SortPointsTest::threePointsCounterClockwise));

    suite->addTest(new CppUnit::TestCaller<SortPointsTest>("fourPointsClockwise", &SortPointsTest::fourPointsClockwise));
    suite->addTest(new CppUnit::TestCaller<SortPointsTest>("fourPointsCounterClockwiseZNormal", &SortPointsTest::fourPointsCounterClockwiseZNormal));
    suite->addTest(new CppUnit::TestCaller<SortPointsTest>("fourPointsCounterClockwiseYNormal", &SortPointsTest::fourPointsCounterClockwiseYNormal));

    return suite;
}

