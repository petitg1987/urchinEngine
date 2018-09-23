#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include "UrchinCommon.h"
#include "UrchinAIEngine.h"

#include "MonotonePolygonTest.h"
#include "AssertHelper.h"
using namespace urchin;

void MonotonePolygonTest::singleTriangle()
{
	std::vector<Point2<float>> ccwPolygonPoints = {Point2<float>(-1.0, -1.0), Point2<float>(1.0, 1.0), Point2<float>(-1.0, 1.0)};
	std::vector<unsigned int> endContourIndices = {(unsigned int)ccwPolygonPoints.size()};
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

void MonotonePolygonTest::oneSplitVertex()
{
	std::vector<Point2<float>> ccwPolygonPoints = {Point2<float>(1.0, 2.0), Point2<float>(0.0, 0.0), Point2<float>(1.0, 1.0), Point2<float>(2.0, 0.0)};
	std::vector<unsigned int> endContourIndices = {(unsigned int)ccwPolygonPoints.size()};
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

void MonotonePolygonTest::twoSplitVertex()
{
	std::vector<Point2<float>> ccwPolygonPoints = {
			Point2<float>(1.0, 3.0), Point2<float>(0.0, 0.0), Point2<float>(1.0, 1.0), Point2<float>(2.0, 0.0),
			Point2<float>(3.0, 1.0), Point2<float>(4.0, 0.0), Point2<float>(3.0, 3.0)
	};
	std::vector<unsigned int> endContourIndices = {(unsigned int)ccwPolygonPoints.size()};
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

void MonotonePolygonTest::oneMergeVertex()
{
	std::vector<Point2<float>> ccwPolygonPoints = {Point2<float>(0.0, 2.0), Point2<float>(1.0, 0.0), Point2<float>(2.0, 2.0), Point2<float>(1.0, 1.0)};
	std::vector<unsigned int> endContourIndices = {(unsigned int)ccwPolygonPoints.size()};
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

void MonotonePolygonTest::twoRegularVertex()
{
	std::vector<Point2<float>> ccwPolygonPoints = {Point2<float>(0.0, 2.0), Point2<float>(1.0, 1.0), Point2<float>(0.0, 0.0), Point2<float>(2.0, 1.0)};
	std::vector<unsigned int> endContourIndices = {(unsigned int)ccwPolygonPoints.size()};
	std::vector<std::string> contourNames = {"test"};

	MonotonePolygonAlgorithm monotonePolygonAlgorithm(ccwPolygonPoints, endContourIndices, contourNames);
	std::vector<MonotonePolygon> monotonePolygons = monotonePolygonAlgorithm.createYMonotonePolygons();

	AssertHelper::assertUnsignedInt(monotonePolygons.size(), 1);
	for(unsigned int i=0; i<ccwPolygonPoints.size(); ++i)
	{
		AssertHelper::assertUnsignedInt(monotonePolygons[0].getCcwPoints()[i], i);
	}
}

void MonotonePolygonTest::splitAndMergeVertex()
{
	std::vector<Point2<float>> ccwPolygonPoints = {
			Point2<float>(0.0, 3.0), Point2<float>(0.0, 0.0), Point2<float>(1.0, 1.0), Point2<float>(2.0, 0.0), Point2<float>(2.0, 3.0), Point2<float>(1.0, 2.0)
	};
	std::vector<unsigned int> endContourIndices = {(unsigned int)ccwPolygonPoints.size()};
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

void MonotonePolygonTest::polygonOneHole()
{
	std::vector<Point2<float>> polygonPoints = {
		Point2<float>(0.0, 0.0), Point2<float>(3.0, 0.0), Point2<float>(3.0, 3.0), Point2<float>(0.0, 3.0), //polygon points
		Point2<float>(1.0, 1.0), Point2<float>(1.0, 2.0), Point2<float>(2.0, 2.0), Point2<float>(2.0, 1.0) //hole points
	};
	std::vector<unsigned int> endContourIndices = {4, (unsigned int)polygonPoints.size()};
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

void MonotonePolygonTest::polygonTwoHoles1()
{
	std::vector<Point2<float>> polygonPoints = {
		Point2<float>(0.0, 0.0), Point2<float>(5.0, 0.0), Point2<float>(5.0, 5.0), Point2<float>(0.0, 5.0), //polygon points
		Point2<float>(1.0, 1.0), Point2<float>(2.0, 2.0), Point2<float>(2.0, 1.0), //hole 1 points
		Point2<float>(4.0, 3.0), Point2<float>(3.0, 4.0), Point2<float>(4.0, 4.0) //hole 2 points
	};
	std::vector<unsigned int> endContourIndices = {4, 7, (unsigned int)polygonPoints.size()};
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

void MonotonePolygonTest::polygonTwoHoles2()
{ //see polygonTwoHoles2.ggb
	std::vector<Point2<float>> polygonPoints = {
			Point2<float>(52.2733727, 28.6202717), Point2<float>(53.4479446, 26.9843941), Point2<float>(54.7769241, 28.4795456), Point2<float>(53.6023521, 30.1154232), //polygon points
			Point2<float>(52.2792969, 28.6201172), Point2<float>(52.609375, 28.9912109), Point2<float>(52.4365234, 28.4033203), //hole 1 points
			Point2<float>(53.8388672, 29.7792969), Point2<float>(54.234375, 29.2304688), Point2<float>(54.0556641, 28.6269531), Point2<float>(53.8388672, 28.6269531) //hole 2 points
	};
	std::vector<unsigned int> endContourIndices = {4, 7, (unsigned int)polygonPoints.size()};
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

void MonotonePolygonTest::polygonTwoHoles3()
{ //see polygonTwoHoles3.ggb
	std::vector<Point2<float>> polygonPoints = {
			//polygon points:
			Point2<float>(70.5370255, 62.2370338), Point2<float>(-67.5370255, 62.2370338), Point2<float>(-67.5370255, -75.8370361), Point2<float>(70.5370255, -75.8370361),
			//hole 1 points:
			Point2<float>(2.56617999, -1.66957319), Point2<float>(-2.56617999, -1.66957319), Point2<float>(-2.56617999, 1.66957319), Point2<float>(2.56617999, 1.66957319),
			//hole 2 points:
			Point2<float>(0.071179986, -6.55628586), Point2<float>(0.0711799115, -6.55628586), Point2<float>(-0.076661095, -5.97861147), Point2<float>(-0.105679169, -5.97032928),
			Point2<float>(-1.07951796, -6.02145529), Point2<float>(-1.14598572, -4.24272919), Point2<float>(0.224839747, -4.17076159), Point2<float>(0.224839762, -4.17076159),
			Point2<float>(0.23582153, -4.46464157), Point2<float>(0.383214414, -4.46464157), Point2<float>(0.383214414, -4.6838522), Point2<float>(0.898347199, -4.83087635),
			Point2<float>(2.76441312, -4.29339933), Point2<float>(3.20787191, -5.65283394),
	};
	std::vector<unsigned int> endContourIndices = {4, 8, (unsigned int)polygonPoints.size()};
	std::vector<std::string> contourNames = {"test", "test", "test"};

	MonotonePolygonAlgorithm monotonePolygonAlgorithm(polygonPoints, endContourIndices, contourNames);
	std::vector<MonotonePolygon> monotonePolygons = monotonePolygonAlgorithm.createYMonotonePolygons();

	//check duplicate points:
	for(const auto &monotonePolygon : monotonePolygons)
	{
		std::set<unsigned int> pointsSet(monotonePolygon.getCcwPoints().begin(), monotonePolygon.getCcwPoints().end());
		AssertHelper::assertUnsignedInt(monotonePolygon.getCcwPoints().size(), pointsSet.size());
	}
}

CppUnit::Test *MonotonePolygonTest::suite()
{
	CppUnit::TestSuite *suite = new CppUnit::TestSuite("MonotonePolygonTest");

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
