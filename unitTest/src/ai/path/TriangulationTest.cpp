#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include "UrchinCommon.h"
#include "UrchinAIEngine.h"

#include "AssertHelper.h"
#include "TriangulationTest.h"
using namespace urchin;

void TriangulationTest::uniqueSplitVertex()
{
	Point2<float> ccwPolygonPointsTab[] = {
			Point2<float>(1.0, 2.0), Point2<float>(0.0, 0.0), Point2<float>(1.0, 1.0), Point2<float>(2.0, 0.0)
	};
	std::vector<Point2<float>> ccwPolygonPoints(ccwPolygonPointsTab, ccwPolygonPointsTab+sizeof(ccwPolygonPointsTab)/sizeof(Point2<float>));

	Triangulation triangulation(ccwPolygonPoints);
	triangulation.triangulate();

	//TODO check connection between 2 & 0
}

void TriangulationTest::uniqueMergeVertex()
{
	Point2<float> ccwPolygonPointsTab[] = {
			Point2<float>(0.0, 2.0), Point2<float>(1.0, 0.0), Point2<float>(2.0, 2.0), Point2<float>(1.0, 1.0)
	};
	std::vector<Point2<float>> ccwPolygonPoints(ccwPolygonPointsTab, ccwPolygonPointsTab+sizeof(ccwPolygonPointsTab)/sizeof(Point2<float>));

	Triangulation triangulation(ccwPolygonPoints);
	triangulation.triangulate();

	//TODO check connection between 1 & 3
}

CppUnit::Test *TriangulationTest::suite()
{
	CppUnit::TestSuite *suite = new CppUnit::TestSuite("TriangulationTest");

	suite->addTest(new CppUnit::TestCaller<TriangulationTest>("uniqueSplitVertex", &TriangulationTest::uniqueSplitVertex));
//	suite->addTest(new CppUnit::TestCaller<TriangulationTest>("uniqueMergeVertex", &TriangulationTest::uniqueMergeVertex));

	return suite;
}
