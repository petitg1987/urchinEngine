#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include "UrchinCommon.h"
#include "UrchinAIEngine.h"

#include "PolygonsIntersectionTest.h"
#include "AssertHelper.h"
using namespace urchin;

void PolygonsIntersectionTest::subjectCoverClipper()
{
	std::vector<Point2<float>> subjectPoly;
	subjectPoly.push_back(Point2<float>(4.0, -4.0));
	subjectPoly.push_back(Point2<float>(-4.0, 4.0));
	subjectPoly.push_back(Point2<float>(4.0, 4.0));

	std::vector<Point2<float>> clipperPoly;
	clipperPoly.push_back(Point2<float>(1.0, 1.0));
	clipperPoly.push_back(Point2<float>(1.0, 3.0));
	clipperPoly.push_back(Point2<float>(3.0, 3.0));
	clipperPoly.push_back(Point2<float>(3.0, 1.0));

	CSGPolygon polygonIntersection = PolygonsIntersection::instance()->intersectionPolygons(CSGPolygon("subject", subjectPoly), CSGPolygon("clipper", clipperPoly));

	AssertHelper::assertUnsignedInt(polygonIntersection.getCwPoints().size(), 4);
	AssertHelper::assertPoint2FloatEquals(polygonIntersection.getCwPoints()[0], Point2<float>(3.0, 3.0));
	AssertHelper::assertPoint2FloatEquals(polygonIntersection.getCwPoints()[1], Point2<float>(3.0, 1.0));
	AssertHelper::assertPoint2FloatEquals(polygonIntersection.getCwPoints()[2], Point2<float>(1.0, 1.0));
	AssertHelper::assertPoint2FloatEquals(polygonIntersection.getCwPoints()[3], Point2<float>(1.0, 3.0));
}

void PolygonsIntersectionTest::clipperCoverSubject()
{
	std::vector<Point2<float>> subjectPoly;
	subjectPoly.push_back(Point2<float>(1.5, 1.5));
	subjectPoly.push_back(Point2<float>(1.5, 2.5));
	subjectPoly.push_back(Point2<float>(2.5, 1.5));

	std::vector<Point2<float>> clipperPoly;
	clipperPoly.push_back(Point2<float>(1.0, 1.0));
	clipperPoly.push_back(Point2<float>(1.0, 3.0));
	clipperPoly.push_back(Point2<float>(3.0, 3.0));
	clipperPoly.push_back(Point2<float>(3.0, 1.0));

	CSGPolygon polygonIntersection = PolygonsIntersection::instance()->intersectionPolygons(CSGPolygon("subject", subjectPoly), CSGPolygon("clipper", clipperPoly));

	AssertHelper::assertUnsignedInt(polygonIntersection.getCwPoints().size(), 3);
	AssertHelper::assertPoint2FloatEquals(polygonIntersection.getCwPoints()[0], Point2<float>(1.5, 1.5));
	AssertHelper::assertPoint2FloatEquals(polygonIntersection.getCwPoints()[1], Point2<float>(1.5, 2.5));
	AssertHelper::assertPoint2FloatEquals(polygonIntersection.getCwPoints()[2], Point2<float>(2.5, 1.5));
}

void PolygonsIntersectionTest::subjectClippedByBox()
{
	std::vector<Point2<float>> subjectPoly;
	subjectPoly.push_back(Point2<float>(1.0, 0.0));
	subjectPoly.push_back(Point2<float>(2.0, 2.0));
	subjectPoly.push_back(Point2<float>(3.0, 0.0));

	std::vector<Point2<float>> clipperPoly;
	clipperPoly.push_back(Point2<float>(1.0, 1.0));
	clipperPoly.push_back(Point2<float>(1.0, 3.0));
	clipperPoly.push_back(Point2<float>(3.0, 3.0));
	clipperPoly.push_back(Point2<float>(3.0, 1.0));

	CSGPolygon polygonIntersection = PolygonsIntersection::instance()->intersectionPolygons(CSGPolygon("subject", subjectPoly), CSGPolygon("clipper", clipperPoly));

	AssertHelper::assertUnsignedInt(polygonIntersection.getCwPoints().size(), 3);
	AssertHelper::assertPoint2FloatEquals(polygonIntersection.getCwPoints()[0], Point2<float>(1.5, 1.0));
	AssertHelper::assertPoint2FloatEquals(polygonIntersection.getCwPoints()[1], Point2<float>(2.0, 2.0));
	AssertHelper::assertPoint2FloatEquals(polygonIntersection.getCwPoints()[2], Point2<float>(2.5, 1.0));
}

CppUnit::Test *PolygonsIntersectionTest::suite()
{
	CppUnit::TestSuite *suite = new CppUnit::TestSuite("PolygonsIntersectionTest");

	suite->addTest(new CppUnit::TestCaller<PolygonsIntersectionTest>("subjectCoverClipper", &PolygonsIntersectionTest::subjectCoverClipper));
	suite->addTest(new CppUnit::TestCaller<PolygonsIntersectionTest>("clipperCoverSubject", &PolygonsIntersectionTest::clipperCoverSubject));

	suite->addTest(new CppUnit::TestCaller<PolygonsIntersectionTest>("subjectClippedByBox", &PolygonsIntersectionTest::subjectClippedByBox));

	return suite;
}
