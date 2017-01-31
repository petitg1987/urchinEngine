#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include "UrchinCommon.h"
#include "UrchinAIEngine.h"

#include "AssertHelper.h"
#include "TriangulationTest.h"
using namespace urchin;

void TriangulationTest::oneSplitVertex()
{
	Point2<float> ccwPolygonPointsTab[] = {
			Point2<float>(1.0, 2.0), Point2<float>(0.0, 0.0), Point2<float>(1.0, 1.0), Point2<float>(2.0, 0.0)
	};
	std::vector<Point2<float>> ccwPolygonPoints(ccwPolygonPointsTab, ccwPolygonPointsTab+sizeof(ccwPolygonPointsTab)/sizeof(Point2<float>));

	Triangulation triangulation(ccwPolygonPoints);
	triangulation.triangulate();

	//TODO check connection between 2 & 0
}

void TriangulationTest::oneMergeVertex()
{
	Point2<float> ccwPolygonPointsTab[] = {
			Point2<float>(0.0, 2.0), Point2<float>(1.0, 0.0), Point2<float>(2.0, 2.0), Point2<float>(1.0, 1.0)
	};
	std::vector<Point2<float>> ccwPolygonPoints(ccwPolygonPointsTab, ccwPolygonPointsTab+sizeof(ccwPolygonPointsTab)/sizeof(Point2<float>));

	Triangulation triangulation(ccwPolygonPoints);
	triangulation.triangulate();

	//TODO check connection between 1 & 3
}

void TriangulationTest::twoRegularVertex()
{
	Point2<float> ccwPolygonPointsTab[] = {
			Point2<float>(0.0, 2.0), Point2<float>(1.0, 1.0), Point2<float>(0.0, 0.0), Point2<float>(2.0, 1.0)
	};
	std::vector<Point2<float>> ccwPolygonPoints(ccwPolygonPointsTab, ccwPolygonPointsTab+sizeof(ccwPolygonPointsTab)/sizeof(Point2<float>));

	Triangulation triangulation(ccwPolygonPoints);
	triangulation.triangulate();

	//TODO check no connection
}

void TriangulationTest::splitAndMergeVertex()
{
	Point2<float> ccwPolygonPointsTab[] = {
			Point2<float>(0.0, 3.0), Point2<float>(0.0, 0.0), Point2<float>(1.0, 1.0), Point2<float>(2.0, 0.0),
			Point2<float>(2.0, 3.0), Point2<float>(1.0, 2.0)
	};
	std::vector<Point2<float>> ccwPolygonPoints(ccwPolygonPointsTab, ccwPolygonPointsTab+sizeof(ccwPolygonPointsTab)/sizeof(Point2<float>));

	Triangulation triangulation(ccwPolygonPoints);
	triangulation.triangulate();

	//TODO check connection between 2 & 5
}

CppUnit::Test *TriangulationTest::suite()
{
	CppUnit::TestSuite *suite = new CppUnit::TestSuite("TriangulationTest");

	suite->addTest(new CppUnit::TestCaller<TriangulationTest>("oneSplitVertex", &TriangulationTest::oneSplitVertex));
	suite->addTest(new CppUnit::TestCaller<TriangulationTest>("oneMergeVertex", &TriangulationTest::oneMergeVertex));
	suite->addTest(new CppUnit::TestCaller<TriangulationTest>("twoRegularVertex", &TriangulationTest::twoRegularVertex));
	suite->addTest(new CppUnit::TestCaller<TriangulationTest>("splitAndMergeVertex", &TriangulationTest::splitAndMergeVertex));

	return suite;
}
