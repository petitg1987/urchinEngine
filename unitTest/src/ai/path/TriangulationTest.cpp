#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include "UrchinCommon.h"
#include "UrchinAIEngine.h"

#include "AssertHelper.h"
#include "TriangulationTest.h"
using namespace urchin;

void TriangulationTest::simpleTriangulation()
{ //TODO complete
	Point2<float> ccwPolygonPointsTab[] = {
			Point2<float>(2.0, 0.0), Point2<float>(1.0, 1.0), Point2<float>(0.0, 0.0)
	};
	std::vector<Point2<float>> ccwPolygonPoints(ccwPolygonPointsTab, ccwPolygonPointsTab+sizeof(ccwPolygonPointsTab)/sizeof(Point2<float>));

	Triangulation triangulation(ccwPolygonPoints);
	triangulation.triangulate();
}

CppUnit::Test *TriangulationTest::suite()
{
	CppUnit::TestSuite *suite = new CppUnit::TestSuite("TriangulationTest");

	suite->addTest(new CppUnit::TestCaller<TriangulationTest>("simpleTriangulation", &TriangulationTest::simpleTriangulation));

	return suite;
}
