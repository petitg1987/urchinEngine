#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include "UrchinCommon.h"
#include "UrchinAIEngine.h"

#include "MonotonePolygonTest.h"
#include "AssertHelper.h"
using namespace urchin;

void MonotonePolygonTest::singleTriangle() {
    std::vector<Point2<float>> ccwPolygonPoints = {Point2<float>(-1.0f, -1.0f), Point2<float>(1.0f, 1.0f), Point2<float>(-1.0f, 1.0f)};
    std::vector<std::size_t> endContourIndices = {ccwPolygonPoints.size()};
    std::vector<std::string> contourNames = {"test"};

    MonotonePolygonAlgorithm monotonePolygonAlgorithm(ccwPolygonPoints, endContourIndices, contourNames);
    std::vector<MonotonePolygon> monotonePolygons = monotonePolygonAlgorithm.createYMonotonePolygons();

    AssertHelper::assertUnsignedInt(monotonePolygons.size(), 1);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints().size(), 3);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints()[0], 0);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints()[1], 1);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints()[2], 2);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getSharedEdges().size(), 0);
}

void MonotonePolygonTest::oneSplitVertex() {
    std::vector<Point2<float>> ccwPolygonPoints = {Point2<float>(1.0f, 2.0f), Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(2.0f, 0.0f)};
    std::vector<std::size_t> endContourIndices = {ccwPolygonPoints.size()};
    std::vector<std::string> contourNames = {"test"};

    MonotonePolygonAlgorithm monotonePolygonAlgorithm(ccwPolygonPoints, endContourIndices, contourNames);
    std::vector<MonotonePolygon> monotonePolygons = monotonePolygonAlgorithm.createYMonotonePolygons();

    AssertHelper::assertUnsignedInt(monotonePolygons.size(), 2);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints().size(), 3);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints()[0], 0);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints()[1], 2);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints()[2], 3);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getSharedEdges().size(), 1);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getCcwPoints().size(), 3);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getCcwPoints()[0], 2);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getCcwPoints()[1], 0);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getCcwPoints()[2], 1);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getSharedEdges().size(), 1);
}

void MonotonePolygonTest::twoSplitVertex() {
    std::vector<Point2<float>> ccwPolygonPoints = {
            Point2<float>(1.0f, 3.0f), Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(2.0f, 0.0f),
            Point2<float>(3.0f, 1.0f), Point2<float>(4.0f, 0.0f), Point2<float>(3.0f, 3.0f)
    };
    std::vector<std::size_t> endContourIndices = {ccwPolygonPoints.size()};
    std::vector<std::string> contourNames = {"test"};

    MonotonePolygonAlgorithm monotonePolygonAlgorithm(ccwPolygonPoints, endContourIndices, contourNames);
    std::vector<MonotonePolygon> monotonePolygons = monotonePolygonAlgorithm.createYMonotonePolygons();

    AssertHelper::assertUnsignedInt(monotonePolygons.size(), 3);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints().size(), 4);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints()[0], 2);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints()[1], 6);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints()[2], 0);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints()[3], 1);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getSharedEdges().size(), 1);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getCcwPoints().size(), 4);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getCcwPoints()[0], 2);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getCcwPoints()[1], 4);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getCcwPoints()[2], 5);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getCcwPoints()[3], 6);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getSharedEdges().size(), 2);
    AssertHelper::assertUnsignedInt(monotonePolygons[2].getCcwPoints().size(), 3);
    AssertHelper::assertUnsignedInt(monotonePolygons[2].getCcwPoints()[0], 4);
    AssertHelper::assertUnsignedInt(monotonePolygons[2].getCcwPoints()[1], 2);
    AssertHelper::assertUnsignedInt(monotonePolygons[2].getCcwPoints()[2], 3);
    AssertHelper::assertUnsignedInt(monotonePolygons[2].getSharedEdges().size(), 1);
}

void MonotonePolygonTest::oneMergeVertex() {
    std::vector<Point2<float>> ccwPolygonPoints = {Point2<float>(0.0f, 2.0f), Point2<float>(1.0f, 0.0f), Point2<float>(2.0f, 2.0f), Point2<float>(1.0f, 1.0f)};
    std::vector<std::size_t> endContourIndices = {ccwPolygonPoints.size()};
    std::vector<std::string> contourNames = {"test"};

    MonotonePolygonAlgorithm monotonePolygonAlgorithm(ccwPolygonPoints, endContourIndices, contourNames);
    std::vector<MonotonePolygon> monotonePolygons = monotonePolygonAlgorithm.createYMonotonePolygons();

    AssertHelper::assertUnsignedInt(monotonePolygons.size(), 2);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints().size(), 3);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints()[0], 1);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints()[1], 3);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints()[2], 0);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getSharedEdges().size(), 1);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getCcwPoints().size(), 3);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getCcwPoints()[0], 3);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getCcwPoints()[1], 1);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getCcwPoints()[2], 2);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getSharedEdges().size(), 1);
}

void MonotonePolygonTest::twoRegularVertex() {
    std::vector<Point2<float>> ccwPolygonPoints = {Point2<float>(0.0f, 2.0f), Point2<float>(1.0f, 1.0f), Point2<float>(0.0f, 0.0f), Point2<float>(2.0f, 1.0f)};
    std::vector<std::size_t> endContourIndices = {ccwPolygonPoints.size()};
    std::vector<std::string> contourNames = {"test"};

    MonotonePolygonAlgorithm monotonePolygonAlgorithm(ccwPolygonPoints, endContourIndices, contourNames);
    std::vector<MonotonePolygon> monotonePolygons = monotonePolygonAlgorithm.createYMonotonePolygons();

    AssertHelper::assertUnsignedInt(monotonePolygons.size(), 1);
    for (std::size_t i = 0; i < ccwPolygonPoints.size(); ++i) {
        AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints()[i], i);
    }
}

void MonotonePolygonTest::splitAndMergeVertex() {
    std::vector<Point2<float>> ccwPolygonPoints = {
            Point2<float>(0.0f, 3.0f), Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(2.0f, 0.0f), Point2<float>(2.0f, 3.0f), Point2<float>(1.0f, 2.0f)
    };
    std::vector<std::size_t> endContourIndices = {ccwPolygonPoints.size()};
    std::vector<std::string> contourNames = {"test"};

    MonotonePolygonAlgorithm monotonePolygonAlgorithm(ccwPolygonPoints, endContourIndices, contourNames);
    std::vector<MonotonePolygon> monotonePolygons = monotonePolygonAlgorithm.createYMonotonePolygons();

    AssertHelper::assertUnsignedInt(monotonePolygons.size(), 2);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints().size(), 4);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints()[0], 2);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints()[1], 5);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints()[2], 0);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints()[3], 1);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getSharedEdges().size(), 1);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getCcwPoints().size(), 4);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getCcwPoints()[0], 5);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getCcwPoints()[1], 2);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getCcwPoints()[2], 3);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getCcwPoints()[3], 4);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getSharedEdges().size(), 1);
}

void MonotonePolygonTest::polygonOneHole() {
    std::vector<Point2<float>> polygonPoints = {
        Point2<float>(0.0f, 0.0f), Point2<float>(3.0f, 0.0f), Point2<float>(3.0f, 3.0f), Point2<float>(0.0f, 3.0f), //polygon points
        Point2<float>(1.0f, 1.0f), Point2<float>(1.0f, 2.0f), Point2<float>(2.0f, 2.0f), Point2<float>(2.0f, 1.0f) //hole points
    };
    std::vector<std::size_t> endContourIndices = {4, polygonPoints.size()};
    std::vector<std::string> contourNames = {"test", "test"};

    MonotonePolygonAlgorithm monotonePolygonAlgorithm(polygonPoints, endContourIndices, contourNames);
    std::vector<MonotonePolygon> monotonePolygons = monotonePolygonAlgorithm.createYMonotonePolygons();

    AssertHelper::assertUnsignedInt(monotonePolygons.size(), 2);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints().size(), 6);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints()[0], 0);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints()[1], 7);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints()[2], 4);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints()[3], 5);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints()[4], 2);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints()[5], 3);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getSharedEdges().size(), 2);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getCcwPoints().size(), 6);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getCcwPoints()[0], 2);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getCcwPoints()[1], 5);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getCcwPoints()[2], 6);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getCcwPoints()[3], 7);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getCcwPoints()[4], 0);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getCcwPoints()[5], 1);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getSharedEdges().size(), 2);
}

void MonotonePolygonTest::polygonTwoHoles1() {
    std::vector<Point2<float>> polygonPoints = {
        Point2<float>(0.0f, 0.0f), Point2<float>(5.0f, 0.0f), Point2<float>(5.0f, 5.0f), Point2<float>(0.0f, 5.0f), //polygon points
        Point2<float>(1.0f, 1.0f), Point2<float>(2.0f, 2.0f), Point2<float>(2.0f, 1.0f), //hole 1 points
        Point2<float>(4.0f, 3.0f), Point2<float>(3.0f, 4.0f), Point2<float>(4.0f, 4.0f) //hole 2 points
    };
    std::vector<std::size_t> endContourIndices = {4, 7, polygonPoints.size()};
    std::vector<std::string> contourNames = {"test", "test", "test"};

    MonotonePolygonAlgorithm monotonePolygonAlgorithm(polygonPoints, endContourIndices, contourNames);
    std::vector<MonotonePolygon> monotonePolygons = monotonePolygonAlgorithm.createYMonotonePolygons();

    AssertHelper::assertUnsignedInt(monotonePolygons.size(), 2);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints().size(), 8);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints()[0], 0);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints()[1], 6);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints()[2], 4);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints()[3], 5);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints()[4], 7);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints()[5], 8);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints()[6], 2);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints()[7], 3);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getCcwPoints().size(), 8);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getCcwPoints()[0], 2);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getCcwPoints()[1], 8);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getCcwPoints()[2], 9);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getCcwPoints()[3], 7);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getCcwPoints()[4], 5);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getCcwPoints()[5], 6);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getCcwPoints()[6], 0);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getCcwPoints()[7], 1);
}

void MonotonePolygonTest::polygonTwoHoles2() { //see monotonePolygonTwoHoles2.ggb
    std::vector<Point2<float>> polygonPoints = {
            Point2<float>(52.2733727f, 28.6202717f), Point2<float>(53.4479446f, 26.9843941f), Point2<float>(54.7769241f, 28.4795456f), Point2<float>(53.6023521f, 30.1154232f), //polygon points
            Point2<float>(52.2792969f, 28.6201172f), Point2<float>(52.609375f, 28.9912109f), Point2<float>(52.4365234f, 28.4033203f), //hole 1 points
            Point2<float>(53.8388672f, 29.7792969f), Point2<float>(54.234375f, 29.2304688f), Point2<float>(54.0556641f, 28.6269531f), Point2<float>(53.8388672f, 28.6269531f) //hole 2 points
    };
    std::vector<std::size_t> endContourIndices = {4, 7, polygonPoints.size()};
    std::vector<std::string> contourNames = {"test", "test", "test"};

    MonotonePolygonAlgorithm monotonePolygonAlgorithm(polygonPoints, endContourIndices, contourNames);
    std::vector<MonotonePolygon> monotonePolygons = monotonePolygonAlgorithm.createYMonotonePolygons();

    AssertHelper::assertUnsignedInt(monotonePolygons.size(), 3);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints().size(), 7);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints()[0], 1);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints()[1], 6);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints()[2], 4);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints()[3], 5);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints()[4], 7);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints()[5], 3);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints()[6], 0);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getCcwPoints().size(), 7);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getCcwPoints()[0], 2);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getCcwPoints()[1], 9);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getCcwPoints()[2], 10);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getCcwPoints()[3], 7);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getCcwPoints()[4], 5);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getCcwPoints()[5], 6);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].getCcwPoints()[6], 1);
    AssertHelper::assertUnsignedInt(monotonePolygons[2].getCcwPoints().size(), 5);
    AssertHelper::assertUnsignedInt(monotonePolygons[2].getCcwPoints()[0], 3);
    AssertHelper::assertUnsignedInt(monotonePolygons[2].getCcwPoints()[1], 7);
    AssertHelper::assertUnsignedInt(monotonePolygons[2].getCcwPoints()[2], 8);
    AssertHelper::assertUnsignedInt(monotonePolygons[2].getCcwPoints()[3], 9);
    AssertHelper::assertUnsignedInt(monotonePolygons[2].getCcwPoints()[4], 2);
}

void MonotonePolygonTest::polygonTwoHoles3() { //see monotonePolygonTwoHoles3.ggb
    std::vector<Point2<float>> polygonPoints = {
            //polygon points:
            Point2<float>(70.5370255f, 62.2370338f), Point2<float>(-67.5370255f, 62.2370338f), Point2<float>(-67.5370255f, -75.8370361f), Point2<float>(70.5370255f, -75.8370361f),
            //hole 1 points:
            Point2<float>(2.56617999f, -1.66957319f), Point2<float>(-2.56617999f, -1.66957319f), Point2<float>(-2.56617999f, 1.66957319f), Point2<float>(2.56617999f, 1.66957319f),
            //hole 2 points:
            Point2<float>(0.071179986f, -6.55628586f), Point2<float>(0.0711799115f, -6.55628586f), Point2<float>(-0.076661095f, -5.97861147f), Point2<float>(-0.105679169f, -5.97032928f),
            Point2<float>(-1.07951796f, -6.02145529f), Point2<float>(-1.14598572f, -4.24272919f), Point2<float>(0.224839747f, -4.17076159f), Point2<float>(0.224839762f, -4.17076159f),
            Point2<float>(0.23582153f, -4.46464157f), Point2<float>(0.383214414f, -4.46464157f), Point2<float>(0.383214414f, -4.6838522f), Point2<float>(0.898347199f, -4.83087635f),
            Point2<float>(2.76441312f, -4.29339933f), Point2<float>(3.20787191f, -5.65283394f),
    };
    std::vector<std::size_t> endContourIndices = {4, 8, polygonPoints.size()};
    std::vector<std::string> contourNames = {"test", "test", "test"};

    MonotonePolygonAlgorithm monotonePolygonAlgorithm(polygonPoints, endContourIndices, contourNames);
    std::vector<MonotonePolygon> monotonePolygons = monotonePolygonAlgorithm.createYMonotonePolygons();

    //check duplicate points:
    for (const auto& monotonePolygon : monotonePolygons) {
        std::set<unsigned int> pointsSet(monotonePolygon.getCcwPoints().begin(), monotonePolygon.getCcwPoints().end());
        AssertHelper::assertUnsignedInt(monotonePolygon.getCcwPoints().size(), pointsSet.size());
    }
}

CppUnit::Test* MonotonePolygonTest::suite() {
    auto* suite = new CppUnit::TestSuite("MonotonePolygonTest");

    suite->addTest(new CppUnit::TestCaller<MonotonePolygonTest>("singleTriangle", &MonotonePolygonTest::singleTriangle));

    suite->addTest(new CppUnit::TestCaller<MonotonePolygonTest>("oneSplitVertex", &MonotonePolygonTest::oneSplitVertex));
    suite->addTest(new CppUnit::TestCaller<MonotonePolygonTest>("twoSplitVertex", &MonotonePolygonTest::twoSplitVertex));
    suite->addTest(new CppUnit::TestCaller<MonotonePolygonTest>("oneMergeVertex", &MonotonePolygonTest::oneMergeVertex));
    suite->addTest(new CppUnit::TestCaller<MonotonePolygonTest>("twoRegularVertex", &MonotonePolygonTest::twoRegularVertex));
    suite->addTest(new CppUnit::TestCaller<MonotonePolygonTest>("splitAndMergeVertex", &MonotonePolygonTest::splitAndMergeVertex));

    suite->addTest(new CppUnit::TestCaller<MonotonePolygonTest>("polygonOneHole", &MonotonePolygonTest::polygonOneHole));
    suite->addTest(new CppUnit::TestCaller<MonotonePolygonTest>("polygonTwoHoles1", &MonotonePolygonTest::polygonTwoHoles1));
    suite->addTest(new CppUnit::TestCaller<MonotonePolygonTest>("polygonTwoHoles2", &MonotonePolygonTest::polygonTwoHoles2));
    suite->addTest(new CppUnit::TestCaller<MonotonePolygonTest>("polygonTwoHoles3", &MonotonePolygonTest::polygonTwoHoles3));

    return suite;
}
