#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <vector>
#include <UrchinCommon.h>

#include <common/math/geometry/3d/util/SortPointsServiceTest.h>
#include <AssertHelper.h>
using namespace urchin;

void SortPointsServiceTest::threePointsClockwise() {
    std::vector<Point3<float>> threePoints({
            Point3<float>(0.0, 0.0, 0.0),
            Point3<float>(0.5, 0.0, -1.0),
            Point3<float>(1.0, 0.0, 0.0)});

    std::vector<Point3<float>> sortedPoints = SortPointsService<float>::sortPointsClockwise(threePoints, Vector3<float>(0.0, -1.0, 0.0));

    AssertHelper::assertPoint3FloatEquals(sortedPoints[0], Point3<float>(0.0, 0.0, 0.0));
    AssertHelper::assertPoint3FloatEquals(sortedPoints[1], Point3<float>(1.0, 0.0, 0.0));
    AssertHelper::assertPoint3FloatEquals(sortedPoints[2], Point3<float>(0.5, 0.0, -1.0));
}

void SortPointsServiceTest::threePointsCounterClockwise() {
    std::vector<Point3<float>> threePoints({
            Point3<float>(0.0, 0.0, 0.0),
            Point3<float>(1.0, 0.0, 0.0),
            Point3<float>(0.5, 0.0, -1.0)});

    std::vector<Point3<float>> sortedPoints = SortPointsService<float>::sortPointsCounterClockwise(threePoints, Vector3<float>(0.0, -1.0, 0.0));

    AssertHelper::assertPoint3FloatEquals(sortedPoints[0], Point3<float>(0.0, 0.0, 0.0));
    AssertHelper::assertPoint3FloatEquals(sortedPoints[1], Point3<float>(0.5, 0.0, -1.0));
    AssertHelper::assertPoint3FloatEquals(sortedPoints[2], Point3<float>(1.0, 0.0, 0.0));
}

void SortPointsServiceTest::fourPointsClockwise() {
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

void SortPointsServiceTest::fourPointsCounterClockwiseZNormal() {
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

void SortPointsServiceTest::fourPointsCounterClockwiseYNormal() {
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

CppUnit::Test* SortPointsServiceTest::suite() {
    auto* suite = new CppUnit::TestSuite("SortPointsServiceTest");

    suite->addTest(new CppUnit::TestCaller<SortPointsServiceTest>("threePointsClockwise", &SortPointsServiceTest::threePointsClockwise));
    suite->addTest(new CppUnit::TestCaller<SortPointsServiceTest>("threePointsCounterClockwise", &SortPointsServiceTest::threePointsCounterClockwise));

    suite->addTest(new CppUnit::TestCaller<SortPointsServiceTest>("fourPointsClockwise", &SortPointsServiceTest::fourPointsClockwise));
    suite->addTest(new CppUnit::TestCaller<SortPointsServiceTest>("fourPointsCounterClockwiseZNormal", &SortPointsServiceTest::fourPointsCounterClockwiseZNormal));
    suite->addTest(new CppUnit::TestCaller<SortPointsServiceTest>("fourPointsCounterClockwiseYNormal", &SortPointsServiceTest::fourPointsCounterClockwiseYNormal));

    return suite;
}

