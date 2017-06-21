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

	MonotonePolygon monotonePolygon(ccwPolygonPoints, endContourIndices);
	std::vector<std::vector<unsigned int>> monotonePolygons = monotonePolygon.createYMonotonePolygons();

	AssertHelper::assertUnsignedInt(monotonePolygons.size(), 1);
	AssertHelper::assertUnsignedInt(monotonePolygons[0].size(), 3);
	AssertHelper::assertUnsignedInt(monotonePolygons[0][0], 0);
	AssertHelper::assertUnsignedInt(monotonePolygons[0][1], 1);
	AssertHelper::assertUnsignedInt(monotonePolygons[0][2], 2);
}

void MonotonePolygonTest::oneSplitVertex()
{
	std::vector<Point2<float>> ccwPolygonPoints = {Point2<float>(1.0, 2.0), Point2<float>(0.0, 0.0), Point2<float>(1.0, 1.0), Point2<float>(2.0, 0.0)};
	std::vector<unsigned int> endContourIndices = {(unsigned int)ccwPolygonPoints.size()};

	MonotonePolygon monotonePolygon(ccwPolygonPoints, endContourIndices);
	std::vector<std::vector<unsigned int>> monotonePolygons = monotonePolygon.createYMonotonePolygons();

	AssertHelper::assertUnsignedInt(monotonePolygons.size(), 2);
	AssertHelper::assertUnsignedInt(monotonePolygons[0].size(), 3);
	AssertHelper::assertUnsignedInt(monotonePolygons[0][0], 0);
	AssertHelper::assertUnsignedInt(monotonePolygons[0][1], 2);
	AssertHelper::assertUnsignedInt(monotonePolygons[0][2], 3);
	AssertHelper::assertUnsignedInt(monotonePolygons[1].size(), 3);
	AssertHelper::assertUnsignedInt(monotonePolygons[1][0], 2);
	AssertHelper::assertUnsignedInt(monotonePolygons[1][1], 0);
	AssertHelper::assertUnsignedInt(monotonePolygons[1][2], 1);
}

void MonotonePolygonTest::twoSplitVertex()
{
	std::vector<Point2<float>> ccwPolygonPoints = {
			Point2<float>(1.0, 3.0), Point2<float>(0.0, 0.0), Point2<float>(1.0, 1.0), Point2<float>(2.0, 0.0), Point2<float>(3.0, 1.0), Point2<float>(4.0, 0.0), Point2<float>(3.0, 3.0)
	};
	std::vector<unsigned int> endContourIndices = {(unsigned int)ccwPolygonPoints.size()};

	MonotonePolygon monotonePolygon(ccwPolygonPoints, endContourIndices);
	std::vector<std::vector<unsigned int>> monotonePolygons = monotonePolygon.createYMonotonePolygons();

	AssertHelper::assertUnsignedInt(monotonePolygons.size(), 3);
	AssertHelper::assertUnsignedInt(monotonePolygons[0].size(), 4);
	AssertHelper::assertUnsignedInt(monotonePolygons[0][0], 2);
	AssertHelper::assertUnsignedInt(monotonePolygons[0][1], 6);
	AssertHelper::assertUnsignedInt(monotonePolygons[0][2], 0);
	AssertHelper::assertUnsignedInt(monotonePolygons[0][3], 1);
	AssertHelper::assertUnsignedInt(monotonePolygons[1].size(), 4);
	AssertHelper::assertUnsignedInt(monotonePolygons[1][0], 2);
	AssertHelper::assertUnsignedInt(monotonePolygons[1][1], 4);
	AssertHelper::assertUnsignedInt(monotonePolygons[1][2], 5);
	AssertHelper::assertUnsignedInt(monotonePolygons[1][3], 6);
	AssertHelper::assertUnsignedInt(monotonePolygons[2].size(), 3);
	AssertHelper::assertUnsignedInt(monotonePolygons[2][0], 4);
	AssertHelper::assertUnsignedInt(monotonePolygons[2][1], 2);
	AssertHelper::assertUnsignedInt(monotonePolygons[2][2], 3);
}

void MonotonePolygonTest::oneMergeVertex()
{
	std::vector<Point2<float>> ccwPolygonPoints = {Point2<float>(0.0, 2.0), Point2<float>(1.0, 0.0), Point2<float>(2.0, 2.0), Point2<float>(1.0, 1.0)};
	std::vector<unsigned int> endContourIndices = {(unsigned int)ccwPolygonPoints.size()};

	MonotonePolygon monotonePolygon(ccwPolygonPoints, endContourIndices);
	std::vector<std::vector<unsigned int>> monotonePolygons = monotonePolygon.createYMonotonePolygons();

	AssertHelper::assertUnsignedInt(monotonePolygons.size(), 2);
	AssertHelper::assertUnsignedInt(monotonePolygons[0].size(), 3);
	AssertHelper::assertUnsignedInt(monotonePolygons[0][0], 1);
	AssertHelper::assertUnsignedInt(monotonePolygons[0][1], 3);
	AssertHelper::assertUnsignedInt(monotonePolygons[0][2], 0);
	AssertHelper::assertUnsignedInt(monotonePolygons[1].size(), 3);
	AssertHelper::assertUnsignedInt(monotonePolygons[1][0], 3);
	AssertHelper::assertUnsignedInt(monotonePolygons[1][1], 1);
	AssertHelper::assertUnsignedInt(monotonePolygons[1][2], 2);
}

void MonotonePolygonTest::twoRegularVertex()
{
	std::vector<Point2<float>> ccwPolygonPoints = {Point2<float>(0.0, 2.0), Point2<float>(1.0, 1.0), Point2<float>(0.0, 0.0), Point2<float>(2.0, 1.0)};
	std::vector<unsigned int> endContourIndices = {(unsigned int)ccwPolygonPoints.size()};

	MonotonePolygon monotonePolygon(ccwPolygonPoints, endContourIndices);
	std::vector<std::vector<unsigned int>> monotonePolygons = monotonePolygon.createYMonotonePolygons();

	AssertHelper::assertUnsignedInt(monotonePolygons.size(), 1);
	for(unsigned int i=0; i<ccwPolygonPoints.size(); ++i)
	{
		AssertHelper::assertUnsignedInt(monotonePolygons[0][i], i);
	}
}

void MonotonePolygonTest::splitAndMergeVertex()
{
	std::vector<Point2<float>> ccwPolygonPoints = {
			Point2<float>(0.0, 3.0), Point2<float>(0.0, 0.0), Point2<float>(1.0, 1.0), Point2<float>(2.0, 0.0), Point2<float>(2.0, 3.0), Point2<float>(1.0, 2.0)
	};
	std::vector<unsigned int> endContourIndices = {(unsigned int)ccwPolygonPoints.size()};

	MonotonePolygon monotonePolygon(ccwPolygonPoints, endContourIndices);
	std::vector<std::vector<unsigned int>> monotonePolygons = monotonePolygon.createYMonotonePolygons();

	AssertHelper::assertUnsignedInt(monotonePolygons.size(), 2);
	AssertHelper::assertUnsignedInt(monotonePolygons[0].size(), 4);
	AssertHelper::assertUnsignedInt(monotonePolygons[0][0], 2);
	AssertHelper::assertUnsignedInt(monotonePolygons[0][1], 5);
	AssertHelper::assertUnsignedInt(monotonePolygons[0][2], 0);
	AssertHelper::assertUnsignedInt(monotonePolygons[0][3], 1);
	AssertHelper::assertUnsignedInt(monotonePolygons[1].size(), 4);
	AssertHelper::assertUnsignedInt(monotonePolygons[1][0], 5);
	AssertHelper::assertUnsignedInt(monotonePolygons[1][1], 2);
	AssertHelper::assertUnsignedInt(monotonePolygons[1][2], 3);
	AssertHelper::assertUnsignedInt(monotonePolygons[1][3], 4);
}

void MonotonePolygonTest::polygonOneHole()
{
	std::vector<Point2<float>> polygonPoints = {
		Point2<float>(0.0, 0.0), Point2<float>(3.0, 0.0), Point2<float>(3.0, 3.0), Point2<float>(0.0, 3.0), //polygon points
		Point2<float>(1.0, 1.0), Point2<float>(1.0, 2.0), Point2<float>(2.0, 2.0), Point2<float>(2.0, 1.0) //hole points
	};
	std::vector<unsigned int> endContourIndices = {4, (unsigned int)polygonPoints.size()};

	MonotonePolygon monotonePolygon(polygonPoints, endContourIndices);
	std::vector<std::vector<unsigned int>> monotonePolygons = monotonePolygon.createYMonotonePolygons();

	AssertHelper::assertUnsignedInt(monotonePolygons.size(), 2);
	AssertHelper::assertUnsignedInt(monotonePolygons[0].size(), 6);
	AssertHelper::assertUnsignedInt(monotonePolygons[0][0], 0);
	AssertHelper::assertUnsignedInt(monotonePolygons[0][1], 7);
	AssertHelper::assertUnsignedInt(monotonePolygons[0][2], 4);
	AssertHelper::assertUnsignedInt(monotonePolygons[0][3], 5);
	AssertHelper::assertUnsignedInt(monotonePolygons[0][4], 2);
	AssertHelper::assertUnsignedInt(monotonePolygons[0][5], 3);
	AssertHelper::assertUnsignedInt(monotonePolygons[1].size(), 6);
	AssertHelper::assertUnsignedInt(monotonePolygons[1][0], 2);
	AssertHelper::assertUnsignedInt(monotonePolygons[1][1], 5);
	AssertHelper::assertUnsignedInt(monotonePolygons[1][2], 6);
	AssertHelper::assertUnsignedInt(monotonePolygons[1][3], 7);
	AssertHelper::assertUnsignedInt(monotonePolygons[1][4], 0);
	AssertHelper::assertUnsignedInt(monotonePolygons[1][5], 1);
}

void MonotonePolygonTest::polygonTwoHoles1()
{
	std::vector<Point2<float>> polygonPoints = {
		Point2<float>(0.0, 0.0), Point2<float>(5.0, 0.0), Point2<float>(5.0, 5.0), Point2<float>(0.0, 5.0), //polygon points
		Point2<float>(1.0, 1.0), Point2<float>(2.0, 2.0), Point2<float>(2.0, 1.0), //hole 1 points
		Point2<float>(4.0, 3.0), Point2<float>(3.0, 4.0), Point2<float>(4.0, 4.0) //hole 2 points
	};
	std::vector<unsigned int> endContourIndices = {4, 7, (unsigned int)polygonPoints.size()};

	MonotonePolygon monotonePolygon(polygonPoints, endContourIndices);
	std::vector<std::vector<unsigned int>> monotonePolygons = monotonePolygon.createYMonotonePolygons();

	AssertHelper::assertUnsignedInt(monotonePolygons.size(), 2);
	AssertHelper::assertUnsignedInt(monotonePolygons[0].size(), 8);
	AssertHelper::assertUnsignedInt(monotonePolygons[0][0], 0);
	AssertHelper::assertUnsignedInt(monotonePolygons[0][1], 6);
	AssertHelper::assertUnsignedInt(monotonePolygons[0][2], 4);
	AssertHelper::assertUnsignedInt(monotonePolygons[0][3], 5);
	AssertHelper::assertUnsignedInt(monotonePolygons[0][4], 7);
	AssertHelper::assertUnsignedInt(monotonePolygons[0][5], 8);
	AssertHelper::assertUnsignedInt(monotonePolygons[0][6], 2);
	AssertHelper::assertUnsignedInt(monotonePolygons[0][7], 3);
	AssertHelper::assertUnsignedInt(monotonePolygons[1].size(), 8);
	AssertHelper::assertUnsignedInt(monotonePolygons[1][0], 2);
	AssertHelper::assertUnsignedInt(monotonePolygons[1][1], 8);
	AssertHelper::assertUnsignedInt(monotonePolygons[1][2], 9);
	AssertHelper::assertUnsignedInt(monotonePolygons[1][3], 7);
	AssertHelper::assertUnsignedInt(monotonePolygons[1][4], 5);
	AssertHelper::assertUnsignedInt(monotonePolygons[1][5], 6);
	AssertHelper::assertUnsignedInt(monotonePolygons[1][6], 0);
	AssertHelper::assertUnsignedInt(monotonePolygons[1][7], 1);
}

void MonotonePolygonTest::polygonTwoHoles2()
{
	std::vector<Point2<float>> polygonPoints = {
			Point2<float>(52.2733727, 28.6202717), Point2<float>(53.4479446, 26.9843941), Point2<float>(54.7769241, 28.4795456), Point2<float>(53.6023521, 30.1154232), //polygon points
			Point2<float>(52.2792969, 28.6201172), Point2<float>(52.609375, 28.9912109), Point2<float>(52.4365234, 28.4033203), //hole 1 points
			Point2<float>(53.8388672, 29.7792969), Point2<float>(54.234375, 29.2304688), Point2<float>(54.0556641, 28.6269531), Point2<float>(53.8388672, 28.6269531) //hole 2 points
	};
	std::vector<unsigned int> endContourIndices = {4, 7, (unsigned int)polygonPoints.size()};

	MonotonePolygon monotonePolygon(polygonPoints, endContourIndices);
	std::vector<std::vector<unsigned int>> monotonePolygons = monotonePolygon.createYMonotonePolygons();

	AssertHelper::assertUnsignedInt(monotonePolygons.size(), 3);
    AssertHelper::assertUnsignedInt(monotonePolygons[0].size(), 7);
    AssertHelper::assertUnsignedInt(monotonePolygons[0][0], 1);
    AssertHelper::assertUnsignedInt(monotonePolygons[0][1], 6);
    AssertHelper::assertUnsignedInt(monotonePolygons[0][2], 4);
    AssertHelper::assertUnsignedInt(monotonePolygons[0][3], 5);
    AssertHelper::assertUnsignedInt(monotonePolygons[0][4], 7);
    AssertHelper::assertUnsignedInt(monotonePolygons[0][5], 3);
    AssertHelper::assertUnsignedInt(monotonePolygons[0][6], 0);
    AssertHelper::assertUnsignedInt(monotonePolygons[1].size(), 7);
    AssertHelper::assertUnsignedInt(monotonePolygons[1][0], 2);
    AssertHelper::assertUnsignedInt(monotonePolygons[1][1], 9);
    AssertHelper::assertUnsignedInt(monotonePolygons[1][2], 10);
    AssertHelper::assertUnsignedInt(monotonePolygons[1][3], 7);
    AssertHelper::assertUnsignedInt(monotonePolygons[1][4], 5);
    AssertHelper::assertUnsignedInt(monotonePolygons[1][5], 6);
    AssertHelper::assertUnsignedInt(monotonePolygons[1][6], 1);
    AssertHelper::assertUnsignedInt(monotonePolygons[2].size(), 5);
    AssertHelper::assertUnsignedInt(monotonePolygons[2][0], 3);
    AssertHelper::assertUnsignedInt(monotonePolygons[2][1], 7);
    AssertHelper::assertUnsignedInt(monotonePolygons[2][2], 8);
    AssertHelper::assertUnsignedInt(monotonePolygons[2][3], 9);
    AssertHelper::assertUnsignedInt(monotonePolygons[2][4], 2);
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

	return suite;
}
