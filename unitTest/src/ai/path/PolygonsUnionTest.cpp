#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include "UrchinCommon.h"
#include "UrchinAIEngine.h"

#include "PolygonsUnionTest.h"
#include "AssertHelper.h"
using namespace urchin;

void PolygonsUnionTest::twoPolygonsNoUnion()
{
	std::vector<Point2<float>> polyPoints1;
	polyPoints1.push_back(Point2<float>(0.0, 0.0));
	polyPoints1.push_back(Point2<float>(0.5, 1.0));
	polyPoints1.push_back(Point2<float>(1.0, 0.0));

	std::vector<Point2<float>> polyPoints2;
	polyPoints2.push_back(Point2<float>(2.0, 0.0));
	polyPoints2.push_back(Point2<float>(2.5, 1.0));
	polyPoints2.push_back(Point2<float>(3.0, 0.0));

	std::vector<CSGPolygon> allPolygons = {CSGPolygon(polyPoints1), CSGPolygon(polyPoints2)};
	std::vector<CSGPolygon> polygonUnion = PolygonsUnion().unionPolygons(allPolygons);

	AssertHelper::assertUnsignedInt(polygonUnion.size(), 2);
	AssertHelper::assertUnsignedInt(polygonUnion[0].getCwPoints().size(), 3);
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[0], Point2<float>(0.0, 0.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[1], Point2<float>(0.5, 1.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[2], Point2<float>(1.0, 0.0));
	AssertHelper::assertUnsignedInt(polygonUnion[1].getCwPoints().size(), 3);
	AssertHelper::assertPoint2FloatEquals(polygonUnion[1].getCwPoints()[0], Point2<float>(2.0, 0.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[1].getCwPoints()[1], Point2<float>(2.5, 1.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[1].getCwPoints()[2], Point2<float>(3.0, 0.0));
}

void PolygonsUnionTest::polygonInsideAnother()
{
	std::vector<Point2<float>> polyPoints1;
	polyPoints1.push_back(Point2<float>(0.0, 0.0));
	polyPoints1.push_back(Point2<float>(1.5, 4.0));
	polyPoints1.push_back(Point2<float>(3.0, 0.0));

	std::vector<Point2<float>> polyPoints2;
	polyPoints2.push_back(Point2<float>(1.0, 1.0));
	polyPoints2.push_back(Point2<float>(1.5, 2.0));
	polyPoints2.push_back(Point2<float>(2.0, 1.0));

	std::vector<CSGPolygon> allPolygons = {CSGPolygon(polyPoints1), CSGPolygon(polyPoints2)};
	std::vector<CSGPolygon> polygonUnion = PolygonsUnion().unionPolygons(allPolygons);

	AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
	AssertHelper::assertUnsignedInt(polygonUnion[0].getCwPoints().size(), 3);
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[0], Point2<float>(0.0, 0.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[1], Point2<float>(1.5, 4.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[2], Point2<float>(3.0, 0.0));
}

void PolygonsUnionTest::twoPolygonsUnion()
{
	std::vector<Point2<float>> polyPoints1;
	polyPoints1.push_back(Point2<float>(0.0, 0.0));
	polyPoints1.push_back(Point2<float>(1.0, 1.0));
	polyPoints1.push_back(Point2<float>(2.0, 0.0));

	std::vector<Point2<float>> polyPoints2;
	polyPoints2.push_back(Point2<float>(0.0, 0.5));
	polyPoints2.push_back(Point2<float>(1.0, 1.5));
	polyPoints2.push_back(Point2<float>(2.0, 0.5));

	std::vector<CSGPolygon> allPolygons = {CSGPolygon(polyPoints1), CSGPolygon(polyPoints2)};
	std::vector<CSGPolygon> polygonUnion = PolygonsUnion().unionPolygons(allPolygons);

	AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
	AssertHelper::assertUnsignedInt(polygonUnion[0].getCwPoints().size(), 7);
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[0], Point2<float>(0.0, 0.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[1], Point2<float>(0.5, 0.5));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[2], Point2<float>(0.0, 0.5));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[3], Point2<float>(1.0, 1.5));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[4], Point2<float>(2.0, 0.5));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[5], Point2<float>(1.5, 0.5));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[6], Point2<float>(2.0, 0.0));
}

void PolygonsUnionTest::threePolygonsUnion()
{
	//TODO to implements
}

void PolygonsUnionTest::twoPolygonsUnionAndSeparatePolygon()
{
	//TODO to implements
}

CppUnit::Test *PolygonsUnionTest::suite()
{
	CppUnit::TestSuite *suite = new CppUnit::TestSuite("PolygonsUnionTest");

	suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoPolygonsNoUnion", &PolygonsUnionTest::twoPolygonsNoUnion));
	suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("polygonInsideAnother", &PolygonsUnionTest::polygonInsideAnother));
	suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoPolygonsUnion", &PolygonsUnionTest::twoPolygonsUnion));
	suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("threePolygonsUnion", &PolygonsUnionTest::threePolygonsUnion));
	suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoPolygonsUnionAndSeparatePolygon", &PolygonsUnionTest::twoPolygonsUnionAndSeparatePolygon));

	return suite;
}
