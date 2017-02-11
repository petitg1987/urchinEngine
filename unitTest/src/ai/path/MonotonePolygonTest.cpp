#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include "UrchinCommon.h"
#include "UrchinAIEngine.h"

#include "MonotonePolygonTest.h"
#include "AssertHelper.h"
using namespace urchin;

void MonotonePolygonTest::oneSplitVertex()
{
	Point2<float> ccwPolygonPointsTab[] = {
			Point2<float>(1.0, 2.0), Point2<float>(0.0, 0.0), Point2<float>(1.0, 1.0), Point2<float>(2.0, 0.0)
	};
	std::vector<Point2<float>> ccwPolygonPoints(ccwPolygonPointsTab, ccwPolygonPointsTab+sizeof(ccwPolygonPointsTab)/sizeof(Point2<float>));

	MonotonePolygon monotonePolygon(ccwPolygonPoints);
	std::vector<std::vector<Point2<float>>> monotonePolygons = monotonePolygon.createYMonotonePolygons();

	AssertHelper::assertUnsignedInt(monotonePolygons.size(), 2);
	AssertHelper::assertUnsignedInt(monotonePolygons[0].size(), 3);
	AssertHelper::assertPoint2FloatEquals(monotonePolygons[0][0], Point2<float>(1.0, 2.0));
	AssertHelper::assertPoint2FloatEquals(monotonePolygons[0][1], Point2<float>(1.0, 1.0));
	AssertHelper::assertPoint2FloatEquals(monotonePolygons[0][2], Point2<float>(2.0, 0.0));
	AssertHelper::assertUnsignedInt(monotonePolygons[1].size(), 3);
	AssertHelper::assertPoint2FloatEquals(monotonePolygons[1][0], Point2<float>(1.0, 1.0));
	AssertHelper::assertPoint2FloatEquals(monotonePolygons[1][1], Point2<float>(1.0, 2.0));
	AssertHelper::assertPoint2FloatEquals(monotonePolygons[1][2], Point2<float>(0.0, 0.0));
}

void MonotonePolygonTest::twoSplitVertex()
{
	Point2<float> ccwPolygonPointsTab[] = {
			Point2<float>(1.0, 3.0), Point2<float>(0.0, 0.0), Point2<float>(1.0, 1.0), Point2<float>(2.0, 0.0),
			Point2<float>(3.0, 1.0), Point2<float>(4.0, 0.0), Point2<float>(3.0, 3.0),
	};
	std::vector<Point2<float>> ccwPolygonPoints(ccwPolygonPointsTab, ccwPolygonPointsTab+sizeof(ccwPolygonPointsTab)/sizeof(Point2<float>));

	MonotonePolygon monotonePolygon(ccwPolygonPoints);
	std::vector<std::vector<Point2<float>>> monotonePolygons = monotonePolygon.createYMonotonePolygons();

	AssertHelper::assertUnsignedInt(monotonePolygons.size(), 3);
	AssertHelper::assertUnsignedInt(monotonePolygons[0].size(), 4);
	AssertHelper::assertPoint2FloatEquals(monotonePolygons[0][0], Point2<float>(1.0, 1.0));
	AssertHelper::assertPoint2FloatEquals(monotonePolygons[0][1], Point2<float>(3.0, 3.0));
	AssertHelper::assertPoint2FloatEquals(monotonePolygons[0][2], Point2<float>(1.0, 3.0));
	AssertHelper::assertPoint2FloatEquals(monotonePolygons[0][3], Point2<float>(0.0, 0.0));
	AssertHelper::assertUnsignedInt(monotonePolygons[1].size(), 4);
	AssertHelper::assertPoint2FloatEquals(monotonePolygons[1][0], Point2<float>(1.0, 1.0));
	AssertHelper::assertPoint2FloatEquals(monotonePolygons[1][1], Point2<float>(3.0, 1.0));
	AssertHelper::assertPoint2FloatEquals(monotonePolygons[1][2], Point2<float>(4.0, 0.0));
	AssertHelper::assertPoint2FloatEquals(monotonePolygons[1][3], Point2<float>(3.0, 3.0));
	AssertHelper::assertUnsignedInt(monotonePolygons[2].size(), 3);
	AssertHelper::assertPoint2FloatEquals(monotonePolygons[2][0], Point2<float>(3.0, 1.0));
	AssertHelper::assertPoint2FloatEquals(monotonePolygons[2][1], Point2<float>(1.0, 1.0));
	AssertHelper::assertPoint2FloatEquals(monotonePolygons[2][2], Point2<float>(2.0, 0.0));
}

void MonotonePolygonTest::oneMergeVertex()
{
	Point2<float> ccwPolygonPointsTab[] = {
			Point2<float>(0.0, 2.0), Point2<float>(1.0, 0.0), Point2<float>(2.0, 2.0), Point2<float>(1.0, 1.0)
	};
	std::vector<Point2<float>> ccwPolygonPoints(ccwPolygonPointsTab, ccwPolygonPointsTab+sizeof(ccwPolygonPointsTab)/sizeof(Point2<float>));

	MonotonePolygon monotonePolygon(ccwPolygonPoints);
	std::vector<std::vector<Point2<float>>> monotonePolygons = monotonePolygon.createYMonotonePolygons();

	AssertHelper::assertUnsignedInt(monotonePolygons.size(), 2);
	AssertHelper::assertUnsignedInt(monotonePolygons[0].size(), 3);
	AssertHelper::assertPoint2FloatEquals(monotonePolygons[0][0], Point2<float>(1.0, 0.0));
	AssertHelper::assertPoint2FloatEquals(monotonePolygons[0][1], Point2<float>(1.0, 1.0));
	AssertHelper::assertPoint2FloatEquals(monotonePolygons[0][2], Point2<float>(0.0, 2.0));
	AssertHelper::assertUnsignedInt(monotonePolygons[1].size(), 3);
	AssertHelper::assertPoint2FloatEquals(monotonePolygons[1][0], Point2<float>(1.0, 1.0));
	AssertHelper::assertPoint2FloatEquals(monotonePolygons[1][1], Point2<float>(1.0, 0.0));
	AssertHelper::assertPoint2FloatEquals(monotonePolygons[1][2], Point2<float>(2.0, 2.0));
}

void MonotonePolygonTest::twoRegularVertex()
{
	Point2<float> ccwPolygonPointsTab[] = {
			Point2<float>(0.0, 2.0), Point2<float>(1.0, 1.0), Point2<float>(0.0, 0.0), Point2<float>(2.0, 1.0)
	};
	std::vector<Point2<float>> ccwPolygonPoints(ccwPolygonPointsTab, ccwPolygonPointsTab+sizeof(ccwPolygonPointsTab)/sizeof(Point2<float>));

	MonotonePolygon monotonePolygon(ccwPolygonPoints);
	std::vector<std::vector<Point2<float>>> monotonePolygons = monotonePolygon.createYMonotonePolygons();

	AssertHelper::assertUnsignedInt(monotonePolygons.size(), 1);
	for(unsigned int i=0; i<ccwPolygonPoints.size(); ++i)
	{
		AssertHelper::assertPoint2FloatEquals(monotonePolygons[0][i], ccwPolygonPoints[i]);
	}
}

void MonotonePolygonTest::splitAndMergeVertex()
{
	Point2<float> ccwPolygonPointsTab[] = {
			Point2<float>(0.0, 3.0), Point2<float>(0.0, 0.0), Point2<float>(1.0, 1.0), Point2<float>(2.0, 0.0),
			Point2<float>(2.0, 3.0), Point2<float>(1.0, 2.0)
	};
	std::vector<Point2<float>> ccwPolygonPoints(ccwPolygonPointsTab, ccwPolygonPointsTab+sizeof(ccwPolygonPointsTab)/sizeof(Point2<float>));

	MonotonePolygon monotonePolygon(ccwPolygonPoints);
	std::vector<std::vector<Point2<float>>> monotonePolygons = monotonePolygon.createYMonotonePolygons();

	AssertHelper::assertUnsignedInt(monotonePolygons.size(), 2);
	AssertHelper::assertUnsignedInt(monotonePolygons[0].size(), 4);
	AssertHelper::assertPoint2FloatEquals(monotonePolygons[0][0], Point2<float>(1.0, 1.0));
	AssertHelper::assertPoint2FloatEquals(monotonePolygons[0][1], Point2<float>(1.0, 2.0));
	AssertHelper::assertPoint2FloatEquals(monotonePolygons[0][2], Point2<float>(0.0, 3.0));
	AssertHelper::assertPoint2FloatEquals(monotonePolygons[0][3], Point2<float>(0.0, 0.0));
	AssertHelper::assertUnsignedInt(monotonePolygons[1].size(), 4);
	AssertHelper::assertPoint2FloatEquals(monotonePolygons[1][0], Point2<float>(1.0, 2.0));
	AssertHelper::assertPoint2FloatEquals(monotonePolygons[1][1], Point2<float>(1.0, 1.0));
	AssertHelper::assertPoint2FloatEquals(monotonePolygons[1][2], Point2<float>(2.0, 0.0));
	AssertHelper::assertPoint2FloatEquals(monotonePolygons[1][3], Point2<float>(2.0, 3.0));
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
