#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include "UrchinCommon.h"
#include "UrchinAIEngine.h"

#include "PolygonsUnionTest.h"
#include "AssertHelper.h"
using namespace urchin;

void PolygonsUnionTest::onePolygonUnion()
{
	std::vector<Point2<float>> polyPoints1;
	polyPoints1.push_back(Point2<float>(0.0, 0.0));
	polyPoints1.push_back(Point2<float>(0.5, 1.0));
	polyPoints1.push_back(Point2<float>(1.0, 0.0));

	std::vector<CSGPolygon> allPolygons = {CSGPolygon("p1", polyPoints1)};
	std::vector<CSGPolygon> polygonUnion = PolygonsUnion().unionPolygons(allPolygons);

	AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
	AssertHelper::assertUnsignedInt(polygonUnion[0].getCwPoints().size(), 3);
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[0], Point2<float>(0.0, 0.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[1], Point2<float>(0.5, 1.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[2], Point2<float>(1.0, 0.0));
}

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

	std::vector<CSGPolygon> allPolygons = {CSGPolygon("p1", polyPoints1), CSGPolygon("p2", polyPoints2)};
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

	std::vector<CSGPolygon> allPolygons = {CSGPolygon("p1", polyPoints1), CSGPolygon("p2", polyPoints2)};
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

	std::vector<CSGPolygon> allPolygons = {CSGPolygon("p1", polyPoints1), CSGPolygon("p2", polyPoints2)};
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

void PolygonsUnionTest::twoPolygonsUnionAxisAligned()
{
	std::vector<Point2<float>> polyPoints1;
	polyPoints1.push_back(Point2<float>(-2.0, 2.0));
	polyPoints1.push_back(Point2<float>(2.0, 2.0));
	polyPoints1.push_back(Point2<float>(2.0, -2.0));
	polyPoints1.push_back(Point2<float>(-2.0, -2.0));

	std::vector<Point2<float>> polyPoints2;
	polyPoints2.push_back(Point2<float>(-2.0, 3.0));
	polyPoints2.push_back(Point2<float>(2.0, 3.0));
	polyPoints2.push_back(Point2<float>(2.0, 1.0));
	polyPoints2.push_back(Point2<float>(-2.0, 1.0));

	std::vector<CSGPolygon> allPolygons = {CSGPolygon("p1", polyPoints1), CSGPolygon("p2", polyPoints2)};
	std::vector<CSGPolygon> polygonUnion = PolygonsUnion().unionPolygons(allPolygons);

	AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
	AssertHelper::assertUnsignedInt(polygonUnion[0].getCwPoints().size(), 6);
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[0], Point2<float>(2.0, -2.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[1], Point2<float>(-2.0, -2.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[2], Point2<float>(-2.0, 2.0)); //TODO test to known the correct point
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[3], Point2<float>(-2.0, 3.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[4], Point2<float>(2.0, 3.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[5], Point2<float>(2.0, 2.0)); //TODO test to known the correct point
}

void PolygonsUnionTest::twoPolygonsSameEndPoint()
{
	std::vector<Point2<float>> polyPoints1;
	polyPoints1.push_back(Point2<float>(0.0, 1.0));
	polyPoints1.push_back(Point2<float>(2.0, 1.0));
	polyPoints1.push_back(Point2<float>(1.0, -1.0));

	std::vector<Point2<float>> polyPoints2;
	polyPoints2.push_back(Point2<float>(0.0, 0.0));
	polyPoints2.push_back(Point2<float>(1.0, 0.0));
	polyPoints2.push_back(Point2<float>(1.0, -1.0));

	std::vector<CSGPolygon> allPolygons[2];
	allPolygons[0].push_back(CSGPolygon("p1", polyPoints1));
	allPolygons[0].push_back(CSGPolygon("p2", polyPoints2));

	allPolygons[1].push_back(CSGPolygon("p2", polyPoints2));
	allPolygons[1].push_back(CSGPolygon("p1", polyPoints1));
	for(unsigned int i=0; i<2; ++i)
	{
		std::vector<CSGPolygon> polygonUnion = PolygonsUnion().unionPolygons(allPolygons[i]);
		AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
		AssertHelper::assertUnsignedInt(polygonUnion[0].getCwPoints().size(), 5);
		AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[0], Point2<float>(1.0, -1.0));
		AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[1], Point2<float>(0.0, 0.0));
		AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[2], Point2<float>(0.5, 0.0));
		AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[3], Point2<float>(0.0, 1.0));
		AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[4], Point2<float>(2.0, 1.0));
	}
}

void PolygonsUnionTest::twoPolygonsTouchInOnePoint()
{
	std::vector<Point2<float>> polyPoints1;
	polyPoints1.push_back(Point2<float>(0.0, -1.0));
	polyPoints1.push_back(Point2<float>(0.0, 0.0));
	polyPoints1.push_back(Point2<float>(2.0, 1.0));

	std::vector<Point2<float>> polyPoints2;
	polyPoints2.push_back(Point2<float>(0.0, 0.0));
	polyPoints2.push_back(Point2<float>(0.0, 1.0));
	polyPoints2.push_back(Point2<float>(1.0, 0.0));

	std::vector<CSGPolygon> allPolygons = {CSGPolygon("p1", polyPoints1), CSGPolygon("p2", polyPoints2)};
	std::vector<CSGPolygon> polygonUnion = PolygonsUnion().unionPolygons(allPolygons);

	AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
	AssertHelper::assertUnsignedInt(polygonUnion[0].getCwPoints().size(), 5);
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[0], Point2<float>(0.0, -1.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[1], Point2<float>(0.0, 0.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[2], Point2<float>(0.0, 1.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[3], Point2<float>(1.5, 0.75));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[4], Point2<float>(2.0, 1.0));
}

void PolygonsUnionTest::twoPolygonsNestedTouchInOnePoint()
{
	std::vector<Point2<float>> polyPoints1;
	polyPoints1.push_back(Point2<float>(0.0, 0.0));
	polyPoints1.push_back(Point2<float>(0.0, 2.0));
	polyPoints1.push_back(Point2<float>(2.0, 0.0));

	std::vector<Point2<float>> polyPoints2;
	polyPoints2.push_back(Point2<float>(0.0, 1.0));
	polyPoints2.push_back(Point2<float>(0.5, 1.0));
	polyPoints2.push_back(Point2<float>(0.5, 0.5));

	std::vector<CSGPolygon> allPolygons = {CSGPolygon("p1", polyPoints1), CSGPolygon("p2", polyPoints2)};
	std::vector<CSGPolygon> polygonUnion = PolygonsUnion().unionPolygons(allPolygons);

	AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
	AssertHelper::assertUnsignedInt(polygonUnion[0].getCwPoints().size(), 4);
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[0], Point2<float>(0.0, 0.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[1], Point2<float>(0.0, 1.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[2], Point2<float>(0.0, 2.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[3], Point2<float>(2.0, 0.0));
}

void PolygonsUnionTest::twoIdenticalTriangles()
{
	std::vector<Point2<float>> polyPoints;
	polyPoints.push_back(Point2<float>(0.0, 0.0));
	polyPoints.push_back(Point2<float>(0.5, 1.0));
	polyPoints.push_back(Point2<float>(1.0, 0.0));

	std::vector<CSGPolygon> allPolygons = {CSGPolygon("p1", polyPoints), CSGPolygon("p2", polyPoints)};
	std::vector<CSGPolygon> polygonUnion = PolygonsUnion().unionPolygons(allPolygons);

	AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
	AssertHelper::assertUnsignedInt(polygonUnion[0].getCwPoints().size(), 3);
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[0], Point2<float>(0.0, 0.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[1], Point2<float>(0.5, 1.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[2], Point2<float>(1.0, 0.0));
}

void PolygonsUnionTest::twoIdenticalSquares()
{
	std::vector<Point2<float>> polyPoints;
	polyPoints.push_back(Point2<float>(-0.225821, 5.97582));
	polyPoints.push_back(Point2<float>(1.12582, 5.97582));
	polyPoints.push_back(Point2<float>(1.12582, 4.62418));
	polyPoints.push_back(Point2<float>(-0.225821, 4.62418));

	std::vector<CSGPolygon> allPolygons = {CSGPolygon("p1", polyPoints), CSGPolygon("p2", polyPoints)};
	std::vector<CSGPolygon> polygonUnion = PolygonsUnion().unionPolygons(allPolygons);

	AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
	AssertHelper::assertUnsignedInt(polygonUnion[0].getCwPoints().size(), 4);
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[0], Point2<float>(1.12582, 4.62418));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[1], Point2<float>(-0.225821, 4.62418));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[2], Point2<float>(-0.225821, 5.97582));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[3], Point2<float>(1.12582, 5.97582));
}

void PolygonsUnionTest::threePolygonsUnion()
{
	std::vector<Point2<float>> polyPoints1;
	polyPoints1.push_back(Point2<float>(0.0, 0.0));
	polyPoints1.push_back(Point2<float>(0.0, 2.0));
	polyPoints1.push_back(Point2<float>(2.0, 2.0));
	polyPoints1.push_back(Point2<float>(2.0, 0.0));

	std::vector<Point2<float>> polyPoints2;
	polyPoints2.push_back(Point2<float>(0.5, 1.0));
	polyPoints2.push_back(Point2<float>(1.0, 3.0));
	polyPoints2.push_back(Point2<float>(1.5, 1.0));

	std::vector<Point2<float>> polyPoints3;
	polyPoints3.push_back(Point2<float>(1.0, 0.5));
	polyPoints3.push_back(Point2<float>(1.0, 1.5));
	polyPoints3.push_back(Point2<float>(3.0, 1.0));

	std::vector<CSGPolygon> allPolygons = {CSGPolygon("p1", polyPoints1), CSGPolygon("p2", polyPoints2), CSGPolygon("p3", polyPoints3)};
	std::vector<CSGPolygon> polygonUnion = PolygonsUnion().unionPolygons(allPolygons);

	AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
	AssertHelper::assertUnsignedInt(polygonUnion[0].getCwPoints().size(), 10);
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[0], Point2<float>(0.0, 0.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[1], Point2<float>(0.0, 2.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[2], Point2<float>(0.75, 2.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[3], Point2<float>(1.0, 3.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[4], Point2<float>(1.25, 2.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[5], Point2<float>(2.0, 2.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[6], Point2<float>(2.0, 1.25));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[7], Point2<float>(3.0, 1.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[8], Point2<float>(2.0, 0.75));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[9], Point2<float>(2.0, 0.0));
}

void PolygonsUnionTest::twoPolygonsUnionAndSeparatePolygon()
{
	std::vector<Point2<float>> polyPoints1;
	polyPoints1.push_back(Point2<float>(4.0, 0.0));
	polyPoints1.push_back(Point2<float>(4.0, 2.0));
	polyPoints1.push_back(Point2<float>(6.0, 2.0));
	polyPoints1.push_back(Point2<float>(6.0, 0.0));

	std::vector<Point2<float>> polyPoints2;
	polyPoints2.push_back(Point2<float>(0.5, 1.0));
	polyPoints2.push_back(Point2<float>(1.0, 3.0));
	polyPoints2.push_back(Point2<float>(1.5, 1.0));

	std::vector<Point2<float>> polyPoints3;
	polyPoints3.push_back(Point2<float>(1.0, 0.5));
	polyPoints3.push_back(Point2<float>(1.0, 1.5));
	polyPoints3.push_back(Point2<float>(3.0, 1.0));

	std::vector<CSGPolygon> allPolygons = {CSGPolygon("p1", polyPoints1), CSGPolygon("p2", polyPoints2), CSGPolygon("p3", polyPoints3)};
	std::vector<CSGPolygon> polygonUnion = PolygonsUnion().unionPolygons(allPolygons);

	AssertHelper::assertUnsignedInt(polygonUnion.size(), 2);
	AssertHelper::assertUnsignedInt(polygonUnion[0].getCwPoints().size(), 4);
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[0], Point2<float>(4.0, 0.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[1], Point2<float>(4.0, 2.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[2], Point2<float>(6.0, 2.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[3], Point2<float>(6.0, 0.0));
	AssertHelper::assertUnsignedInt(polygonUnion[1].getCwPoints().size(), 6);
	AssertHelper::assertPoint2FloatEquals(polygonUnion[1].getCwPoints()[0], Point2<float>(1.0, 0.5));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[1].getCwPoints()[1], Point2<float>(1.0, 1.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[1].getCwPoints()[2], Point2<float>(0.5, 1.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[1].getCwPoints()[3], Point2<float>(1.0, 3.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[1].getCwPoints()[4], Point2<float>(1.4, 1.4));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[1].getCwPoints()[5], Point2<float>(3.0, 1.0));
}

CppUnit::Test *PolygonsUnionTest::suite()
{
	CppUnit::TestSuite *suite = new CppUnit::TestSuite("PolygonsUnionTest");

/*	suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("onePolygonUnion", &PolygonsUnionTest::onePolygonUnion));

	suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoPolygonsNoUnion", &PolygonsUnionTest::twoPolygonsNoUnion));
	suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("polygonInsideAnother", &PolygonsUnionTest::polygonInsideAnother));

	suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoPolygonsUnion", &PolygonsUnionTest::twoPolygonsUnion));
	suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoPolygonsUnionAxisAligned", &PolygonsUnionTest::twoPolygonsUnionAxisAligned));
	suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoPolygonsSameEndPoint", &PolygonsUnionTest::twoPolygonsSameEndPoint));*/
	suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoPolygonsTouchInOnePoint", &PolygonsUnionTest::twoPolygonsTouchInOnePoint));
/*	suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoPolygonsNestedTouchInOnePoint", &PolygonsUnionTest::twoPolygonsNestedTouchInOnePoint));

	suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoIdenticalTriangles", &PolygonsUnionTest::twoIdenticalTriangles));
	suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoIdenticalSquares", &PolygonsUnionTest::twoIdenticalSquares));

	suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("threePolygonsUnion", &PolygonsUnionTest::threePolygonsUnion));
	suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoPolygonsUnionAndSeparatePolygon", &PolygonsUnionTest::twoPolygonsUnionAndSeparatePolygon));*/

	return suite;
}
