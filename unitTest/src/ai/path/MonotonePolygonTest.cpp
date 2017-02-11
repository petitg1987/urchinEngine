#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include "UrchinCommon.h"
#include "UrchinAIEngine.h"

#include "MonotonePolygonTest.h"
#include "AssertHelper.h"
using namespace urchin;

void MonotonePolygonTest::oneSplitVertex()
{
	std::vector<Point2<float>> ccwPolygonPoints = {Point2<float>(1.0, 2.0), Point2<float>(0.0, 0.0), Point2<float>(1.0, 1.0), Point2<float>(2.0, 0.0)};

	MonotonePolygon monotonePolygon(ccwPolygonPoints);
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
	std::vector<Point2<float>> ccwPolygonPoints = {Point2<float>(1.0, 3.0), Point2<float>(0.0, 0.0), Point2<float>(1.0, 1.0), Point2<float>(2.0, 0.0),
			Point2<float>(3.0, 1.0), Point2<float>(4.0, 0.0), Point2<float>(3.0, 3.0)};

	MonotonePolygon monotonePolygon(ccwPolygonPoints);
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

	MonotonePolygon monotonePolygon(ccwPolygonPoints);
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

	MonotonePolygon monotonePolygon(ccwPolygonPoints);
	std::vector<std::vector<unsigned int>> monotonePolygons = monotonePolygon.createYMonotonePolygons();

	AssertHelper::assertUnsignedInt(monotonePolygons.size(), 1);
	for(unsigned int i=0; i<ccwPolygonPoints.size(); ++i)
	{
		AssertHelper::assertUnsignedInt(monotonePolygons[0][i], i);
	}
}

void MonotonePolygonTest::splitAndMergeVertex()
{
	std::vector<Point2<float>> ccwPolygonPoints = {Point2<float>(0.0, 3.0), Point2<float>(0.0, 0.0), Point2<float>(1.0, 1.0), Point2<float>(2.0, 0.0),
			Point2<float>(2.0, 3.0), Point2<float>(1.0, 2.0)};

	MonotonePolygon monotonePolygon(ccwPolygonPoints);
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

CppUnit::Test *MonotonePolygonTest::suite()
{
	CppUnit::TestSuite *suite = new CppUnit::TestSuite("MonotonePolygonTest");

	suite->addTest(new CppUnit::TestCaller<MonotonePolygonTest>("oneSplitVertex", &MonotonePolygonTest::oneSplitVertex));
	suite->addTest(new CppUnit::TestCaller<MonotonePolygonTest>("twoSplitVertex", &MonotonePolygonTest::twoSplitVertex));
	suite->addTest(new CppUnit::TestCaller<MonotonePolygonTest>("oneMergeVertex", &MonotonePolygonTest::oneMergeVertex));
	suite->addTest(new CppUnit::TestCaller<MonotonePolygonTest>("twoRegularVertex", &MonotonePolygonTest::twoRegularVertex));
	suite->addTest(new CppUnit::TestCaller<MonotonePolygonTest>("splitAndMergeVertex", &MonotonePolygonTest::splitAndMergeVertex));

	return suite;
}
