#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include "UrchinCommon.h"
#include "UrchinAIEngine.h"

#include "TriangulationTest.h"
#include "AssertHelper.h"
using namespace urchin;


void TriangulationTest::triangleTriangulation()
{
	Point2<float> ccwPolygonPointsTab[] = {
			Point2<float>(0.0, 0.0), Point2<float>(2.0, 0.0), Point2<float>(1.0, 1.0)
	};
	std::vector<Point2<float>> ccwPolygonPoints(ccwPolygonPointsTab, ccwPolygonPointsTab+sizeof(ccwPolygonPointsTab)/sizeof(Point2<float>));

	Triangulation triangulation(ccwPolygonPoints);
	triangulation.triangulate();

	//TODO check triangle: 012
}

void TriangulationTest::cubeTriangulation()
{
	Point2<float> ccwPolygonPointsTab[] = {
			Point2<float>(0.0, 0.0), Point2<float>(1.0, 0.0), Point2<float>(1.0, 1.0), Point2<float>(0.0, 1.0)
	};
	std::vector<Point2<float>> ccwPolygonPoints(ccwPolygonPointsTab, ccwPolygonPointsTab+sizeof(ccwPolygonPointsTab)/sizeof(Point2<float>));

	Triangulation triangulation(ccwPolygonPoints);
	triangulation.triangulate();

	//TODO check triangle: 023, 120
}

void TriangulationTest::cavityTriangulation()
{
	Point2<float> ccwPolygonPointsTab[] = {
			Point2<float>(3.0, 4.0), Point2<float>(0.0, 3.0), Point2<float>(1.0, 2.0), Point2<float>(0.0, 1.75), Point2<float>(2.0, 0.0)
	};
	std::vector<Point2<float>> ccwPolygonPoints(ccwPolygonPointsTab, ccwPolygonPointsTab+sizeof(ccwPolygonPointsTab)/sizeof(Point2<float>));

	Triangulation triangulation(ccwPolygonPoints);
	triangulation.triangulate();

	//TODO check: 201, 423, 402
}

CppUnit::Test *TriangulationTest::suite()
{
	CppUnit::TestSuite *suite = new CppUnit::TestSuite("TriangulationTest");

	suite->addTest(new CppUnit::TestCaller<TriangulationTest>("triangleTriangulation", &TriangulationTest::triangleTriangulation));
	suite->addTest(new CppUnit::TestCaller<TriangulationTest>("cubeTriangulation", &TriangulationTest::cubeTriangulation));
	suite->addTest(new CppUnit::TestCaller<TriangulationTest>("cavityTriangulation", &TriangulationTest::cavityTriangulation));

	return suite;
}
