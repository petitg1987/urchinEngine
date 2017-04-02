#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include "UrchinCommon.h"
#include "UrchinAIEngine.h"

#include "PolygonsUnionTest.h"
#include "AssertHelper.h"
using namespace urchin;

void PolygonsUnionTest::threePolygonsUnion()
{ //TODO review test
	std::vector<Point2<float>> polyPoints1;
	polyPoints1.push_back(Point2<float>(1.0, 1.0));

	std::vector<Point2<float>> polyPoints2;
	polyPoints2.push_back(Point2<float>(1.0, 1.0));

	std::vector<Point2<float>> polyPoints3;
	polyPoints3.push_back(Point2<float>(1.0, 1.0));

	std::vector<CSGPolygon> threePolygons;
	threePolygons.push_back(CSGPolygon(polyPoints1));
	threePolygons.push_back(CSGPolygon(polyPoints2));
	threePolygons.push_back(CSGPolygon(polyPoints3));

	PolygonsUnion polygonsUnion;
	polygonsUnion.unionPolygons(threePolygons);
}

CppUnit::Test *PolygonsUnionTest::suite()
{
	CppUnit::TestSuite *suite = new CppUnit::TestSuite("PolygonsUnionTest");

	suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("threePolygonsUnion", &PolygonsUnionTest::threePolygonsUnion));

	return suite;
}
