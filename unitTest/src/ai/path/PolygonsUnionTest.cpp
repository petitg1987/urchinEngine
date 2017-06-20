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

	std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", polyPoints1)};
	std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

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

	std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", polyPoints1), CSGPolygon<float>("p2", polyPoints2)};
	std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

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

	std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", polyPoints1), CSGPolygon<float>("p2", polyPoints2)};
	std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

	AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
	AssertHelper::assertUnsignedInt(polygonUnion[0].getCwPoints().size(), 3);
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[0], Point2<float>(0.0, 0.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[1], Point2<float>(1.5, 4.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[2], Point2<float>(3.0, 0.0));
}

void PolygonsUnionTest::polygonInsideAnotherTouchInOnePoint()
{
	std::vector<Point2<float>> polyPoints1;
	polyPoints1.push_back(Point2<float>(1.0, 0.0));
	polyPoints1.push_back(Point2<float>(2.0, -0.01));
	polyPoints1.push_back(Point2<float>(2.0, -1.0));

	std::vector<Point2<float>> polyPoints2;
	polyPoints2.push_back(Point2<float>(0.0, 0.0));
	polyPoints2.push_back(Point2<float>(3.0, 0.0));
	polyPoints2.push_back(Point2<float>(0.0, -5.0));

	std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", polyPoints1), CSGPolygon<float>("p2", polyPoints2)};
	std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

	AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[0], Point2<float>(0.0, -5.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[1], Point2<float>(0.0, 0.0));
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

	std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", polyPoints1), CSGPolygon<float>("p2", polyPoints2)};
	std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

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

void PolygonsUnionTest::twoPolygonsUnionXAligned()
{
	std::vector<Point2<float>> polyPoints1;
	polyPoints1.push_back(Point2<float>(-1.52582097, 5.9758215));
	polyPoints1.push_back(Point2<float>(-0.174179018, 5.9758215));
	polyPoints1.push_back(Point2<float>(-0.174179018, 4.62417889));
	polyPoints1.push_back(Point2<float>(-1.52582097, 4.62417889));

	std::vector<Point2<float>> polyPoints2;
	polyPoints2.push_back(Point2<float>(-0.225821018, 5.9758215));
	polyPoints2.push_back(Point2<float>(1.12582099, 5.9758215));
	polyPoints2.push_back(Point2<float>(1.12582099, 4.62417889));
	polyPoints2.push_back(Point2<float>(-0.225821018, 4.62417889));

	std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", polyPoints1), CSGPolygon<float>("p2", polyPoints2)};
	std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

	AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
	AssertHelper::assertUnsignedInt(polygonUnion[0].getCwPoints().size(), 7);
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[0], Point2<float>(1.12582099, 4.62417889));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[1], Point2<float>(-0.174179018, 4.62417889));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[2], Point2<float>(-0.174179018, 4.62417889));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[3], Point2<float>(-1.52582097, 4.62417889));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[4], Point2<float>(-1.52582097, 5.9758215));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[5], Point2<float>(-0.225821018, 5.9758215));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[6], Point2<float>(1.12582099, 5.9758215));
}

void PolygonsUnionTest::twoPolygonsUnionYAligned()
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

	std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", polyPoints1), CSGPolygon<float>("p2", polyPoints2)};
	std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

	AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
	AssertHelper::assertUnsignedInt(polygonUnion[0].getCwPoints().size(), 6);
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[0], Point2<float>(2.0, -2.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[1], Point2<float>(-2.0, -2.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[2], Point2<float>(-2.0, 1.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[3], Point2<float>(-2.0, 3.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[4], Point2<float>(2.0, 3.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[5], Point2<float>(2.0, 2.0));
}

void PolygonsUnionTest::twoPolygonsUnionYAlmostAligned()
{
	std::vector<Point2<float>> polyPoints1;
	polyPoints1.push_back(Point2<float>(-2.61617994, 1.71957302));
	polyPoints1.push_back(Point2<float>(2.61617994, 1.71957302));
	polyPoints1.push_back(Point2<float>(2.61617994, -1.71957302));
	polyPoints1.push_back(Point2<float>(-2.61617994, -1.71957302));

	std::vector<Point2<float>> polyPoints2;
	polyPoints2.push_back(Point2<float>(-2.6161797, 3.36731601));
	polyPoints2.push_back(Point2<float>(2.6161797, 3.36731601));
	polyPoints2.push_back(Point2<float>(2.6161797, 1.63268399));
	polyPoints2.push_back(Point2<float>(-2.6161797, 1.63268399));

	std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", polyPoints1), CSGPolygon<float>("p2", polyPoints2)};
	std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

	AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
	AssertHelper::assertUnsignedInt(polygonUnion[0].getCwPoints().size(), 8);
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[0], Point2<float>(2.61617994, -1.71957302), 0.0000001);
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[1], Point2<float>(-2.61617994, -1.71957302), 0.0000001);
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[2], Point2<float>(-2.61617994, 1.71957302), 0.0000001);
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[3], Point2<float>(-2.6161797, 1.71957302), 0.0000001);
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[4], Point2<float>(-2.6161797, 3.36731601), 0.0000001);
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[5], Point2<float>(2.6161797, 3.36731601), 0.0000001);
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[6], Point2<float>(2.6161797, 1.71957302), 0.0000001);
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[7], Point2<float>(2.61617994, 1.71957302), 0.0000001);
}

void PolygonsUnionTest::twoPolygonsUnionAlmostSameEdge()
{
	std::vector<Point2<int>> polyPoints1;
	polyPoints1.push_back(Point2<int>(-419, -5371));
	polyPoints1.push_back(Point2<int>(912, -5200));
	polyPoints1.push_back(Point2<int>(912, -5780));
	polyPoints1.push_back(Point2<int>(-419, -5780));

	std::vector<Point2<int>> polyPoints2;
	polyPoints2.push_back(Point2<int>(-51, -5324));
	polyPoints2.push_back(Point2<int>(973, -5192));
	polyPoints2.push_back(Point2<int>(973, -5939));
	polyPoints2.push_back(Point2<int>(-51, -5939));

	std::vector<CSGPolygon<int>> allPolygons = {CSGPolygon<int>("p1", polyPoints1), CSGPolygon<int>("p2", polyPoints2)};
	std::vector<CSGPolygon<int>> polygonUnion = PolygonsUnion<int>::instance()->unionPolygons(allPolygons);

	AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
	AssertHelper::assertUnsignedInt(polygonUnion[0].getCwPoints().size(), 7);
	AssertHelper::assertPoint2IntEquals(polygonUnion[0].getCwPoints()[0], Point2<int>(973, -5939));
	AssertHelper::assertPoint2IntEquals(polygonUnion[0].getCwPoints()[1], Point2<int>(-51, -5939));
	AssertHelper::assertPoint2IntEquals(polygonUnion[0].getCwPoints()[2], Point2<int>(-51, -5780));
	AssertHelper::assertPoint2IntEquals(polygonUnion[0].getCwPoints()[3], Point2<int>(-419, -5780));
	AssertHelper::assertPoint2IntEquals(polygonUnion[0].getCwPoints()[4], Point2<int>(-419, -5371));
	AssertHelper::assertPoint2IntEquals(polygonUnion[0].getCwPoints()[5], Point2<int>(595, -5241));
	AssertHelper::assertPoint2IntEquals(polygonUnion[0].getCwPoints()[6], Point2<int>(973, -5192));
}

void PolygonsUnionTest::twoPolygonsUnionAlmostSameEdge2()
{
    std::vector<Point2<int>> polyPoints1;
    polyPoints1.push_back(Point2<int>(49050, 20245));
    polyPoints1.push_back(Point2<int>(49156, 19704));
    polyPoints1.push_back(Point2<int>(48751, 20265));

    std::vector<Point2<int>> polyPoints2;
    polyPoints2.push_back(Point2<int>(47875, 19997));
    polyPoints2.push_back(Point2<int>(49098, 19997));
    polyPoints2.push_back(Point2<int>(49420, 18354));
    polyPoints2.push_back(Point2<int>(48381, 18423));
    polyPoints2.push_back(Point2<int>(47875, 19124));

    std::vector<CSGPolygon<int>> allPolygons = {CSGPolygon<int>("p1", polyPoints1), CSGPolygon<int>("p2", polyPoints2)};
    std::vector<CSGPolygon<int>> polygonUnion = PolygonsUnion<int>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
    AssertHelper::assertUnsignedInt(polygonUnion[0].getCwPoints().size(), 9);
    //no points check
}

void PolygonsUnionTest::twoPolygonsUnionAlmostSameEdge3()
{
    std::vector<Point2<int>> polyPoints1;
    polyPoints1.push_back(Point2<int>(922, -5526));
    polyPoints1.push_back(Point2<int>(922, -5786));
    polyPoints1.push_back(Point2<int>(-53, -5786));

    std::vector<Point2<int>> polyPoints2;
    polyPoints2.push_back(Point2<int>(-51, -5785));
    polyPoints2.push_back(Point2<int>(973, -5512));
    polyPoints2.push_back(Point2<int>(973, -5939));
    polyPoints2.push_back(Point2<int>(-51, -5939));

    std::vector<CSGPolygon<int>> allPolygons = {CSGPolygon<int>("p1", polyPoints1), CSGPolygon<int>("p2", polyPoints2)};
    std::vector<CSGPolygon<int>> polygonUnion = PolygonsUnion<int>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
    AssertHelper::assertUnsignedInt(polygonUnion[0].getCwPoints().size(), 6);
    //no points check
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

	std::vector<CSGPolygon<float>> allPolygons[2];
	allPolygons[0].push_back(CSGPolygon<float>("p1", polyPoints1));
	allPolygons[0].push_back(CSGPolygon<float>("p2", polyPoints2));

	allPolygons[1].push_back(CSGPolygon<float>("p2", polyPoints2));
	allPolygons[1].push_back(CSGPolygon<float>("p1", polyPoints1));
	for(unsigned int i=0; i<2; ++i)
	{
		std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons[i]);
		AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
		AssertHelper::assertUnsignedInt(polygonUnion[0].getCwPoints().size(), 5);
		AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[0], Point2<float>(1.0, -1.0));
		AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[1], Point2<float>(0.0, 0.0));
		AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[2], Point2<float>(0.5, 0.0));
		AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[3], Point2<float>(0.0, 1.0));
		AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[4], Point2<float>(2.0, 1.0));
	}
}

void PolygonsUnionTest::twoPolygonsIntersectionIsEndPoint()
{
	std::vector<Point2<float>> polyPoints1;
	polyPoints1.push_back(Point2<float>(-0.225821018, 5.9758215));
	polyPoints1.push_back(Point2<float>(1.12582099, 5.9758215));
	polyPoints1.push_back(Point2<float>(1.12582099, 4.62417889));
	polyPoints1.push_back(Point2<float>(-0.225821018, 4.62417889));

	std::vector<Point2<float>> polyPoints2;
	polyPoints2.push_back(Point2<float>(-1.52582097, 5.9758215));
	polyPoints2.push_back(Point2<float>(-0.174179018, 5.9758215));
	polyPoints2.push_back(Point2<float>(-0.174179018, 4.62417889));
	polyPoints2.push_back(Point2<float>(-1.52582097, 4.62417889));

	std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", polyPoints1), CSGPolygon<float>("p2", polyPoints2)};
	std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

	AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
	AssertHelper::assertUnsignedInt(polygonUnion[0].getCwPoints().size(), 6);
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[0], Point2<float>(-0.174179018, 4.62417889));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[1], Point2<float>(-1.52582097, 4.62417889));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[2], Point2<float>(-1.52582097, 5.9758215));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[3], Point2<float>(-0.225821018, 5.9758215));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[4], Point2<float>(1.12582099, 5.9758215));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[5], Point2<float>(1.12582099, 4.62417889));
}


void PolygonsUnionTest::twoPolygonsIntersectionIsAlmostEndPoint()
{
	std::vector<Point2<int>> polyPoints1;
	polyPoints1.push_back(Point2<int>(56659, 28091));
	polyPoints1.push_back(Point2<int>(58406, 28091));
	polyPoints1.push_back(Point2<int>(58406, 26345));
	polyPoints1.push_back(Point2<int>(56659, 26345));

	std::vector<Point2<int>> polyPoints2;
	polyPoints2.push_back(Point2<int>(54788, 28295));
	polyPoints2.push_back(Point2<int>(55031, 29016));
	polyPoints2.push_back(Point2<int>(56989, 29803));
	polyPoints2.push_back(Point2<int>(58457, 28245));
	polyPoints2.push_back(Point2<int>(58010, 26919));
	polyPoints2.push_back(Point2<int>(56614, 26357));

	std::vector<CSGPolygon<int>> allPolygons = {CSGPolygon<int>("p1", polyPoints1), CSGPolygon<int>("p2", polyPoints2)};
	std::vector<CSGPolygon<int>> polygonUnion = PolygonsUnion<int>::instance()->unionPolygons(allPolygons);

	AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
	AssertHelper::assertUnsignedInt(polygonUnion[0].getCwPoints().size(), 10);
	//no points check
}

void PolygonsUnionTest::twoPolygonsIntersectionIsAlmostEdgeA()
{
	//In (-0.224820852, 5.97340727), there is a start point and also an intersection point.
	std::vector<Point2<float>> polyPoints1;
	polyPoints1.push_back(Point2<float>(-0.224820852, -5.97340727));
	polyPoints1.push_back(Point2<float>(-0.224821016, -4.62517881));
	polyPoints1.push_back(Point2<float>(1.12340569, -4.62517881));

	std::vector<Point2<float>> polyPoints2;
	polyPoints2.push_back(Point2<float>(-0.224821031, -5.97340727));
	polyPoints2.push_back(Point2<float>(-0.224821031, -4.62517881));
	polyPoints2.push_back(Point2<float>(0.194670022, -4.62517881));
	polyPoints2.push_back(Point2<float>(0.194670022, -5.55391598));

	std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", polyPoints1), CSGPolygon<float>("p2", polyPoints2)};
	std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

	AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
	AssertHelper::assertUnsignedInt(polygonUnion[0].getCwPoints().size(), 5);
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[0], Point2<float>(-0.224820852, -5.97340727));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[1], Point2<float>(-0.224821031, -5.97340727));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[2], Point2<float>(-0.224821031, -4.62517881));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[3], Point2<float>(-0.224821016, -4.62517881));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[4], Point2<float>(1.12340569, -4.62517881));
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

	std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", polyPoints1), CSGPolygon<float>("p2", polyPoints2)};
	std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

	AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
	AssertHelper::assertUnsignedInt(polygonUnion[0].getCwPoints().size(), 5);
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[0], Point2<float>(0.0, -1.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[1], Point2<float>(0.0, 0.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[2], Point2<float>(0.0, 1.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[3], Point2<float>(0.666666, 0.333333));
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

	std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", polyPoints1), CSGPolygon<float>("p2", polyPoints2)};
	std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

	AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
	AssertHelper::assertUnsignedInt(polygonUnion[0].getCwPoints().size(), 3);
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[0], Point2<float>(0.0, 0.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[1], Point2<float>(0.0, 2.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[2], Point2<float>(2.0, 0.0));
}

void PolygonsUnionTest::twoPolygonsIntersectionLastEdge()
{
	std::vector<Point2<float>> polyPoints1;
	polyPoints1.push_back(Point2<float>(2.0, -0.5));
	polyPoints1.push_back(Point2<float>(0.0, 0.0));
	polyPoints1.push_back(Point2<float>(0.0, 2.0));

	std::vector<Point2<float>> polyPoints2;
	polyPoints2.push_back(Point2<float>(0.5, 0.5));
	polyPoints2.push_back(Point2<float>(0.5, 1.0));
	polyPoints2.push_back(Point2<float>(2.0, 0.5));

	std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", polyPoints1), CSGPolygon<float>("p2", polyPoints2)};
	std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

	AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
	AssertHelper::assertUnsignedInt(polygonUnion[0].getCwPoints().size(), 6);
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[0], Point2<float>(2.0, -0.5));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[1], Point2<float>(0.0, 0.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[2], Point2<float>(0.0, 2.0));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[3], Point2<float>(0.90909, 0.863636));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[4], Point2<float>(2.0, 0.5));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[5], Point2<float>(1.2, 0.5));
}

void PolygonsUnionTest::twoIdenticalTriangles()
{
	std::vector<Point2<float>> polyPoints;
	polyPoints.push_back(Point2<float>(0.0, 0.0));
	polyPoints.push_back(Point2<float>(0.5, 1.0));
	polyPoints.push_back(Point2<float>(1.0, 0.0));

	std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", polyPoints), CSGPolygon<float>("p2", polyPoints)};
	std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

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

	std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", polyPoints), CSGPolygon<float>("p2", polyPoints)};
	std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

	AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
	AssertHelper::assertUnsignedInt(polygonUnion[0].getCwPoints().size(), 4);
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[0], Point2<float>(1.12582, 4.62418));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[1], Point2<float>(-0.225821, 4.62418));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[2], Point2<float>(-0.225821, 5.97582));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[3], Point2<float>(1.12582, 5.97582));
}

void PolygonsUnionTest::twoAlmostIdenticalSquares()
{
	std::vector<Point2<float>> polyPoints1;
	polyPoints1.push_back(Point2<float>(-0.225821018, -4.62417889));
	polyPoints1.push_back(Point2<float>(1.12581992, -4.62417889));
	polyPoints1.push_back(Point2<float>(1.12581992, -5.9758215));
	polyPoints1.push_back(Point2<float>(-0.225821018, -5.9758215));

	std::vector<Point2<float>> polyPoints2;
	polyPoints2.push_back(Point2<float>(-0.225821018, -4.62417889));
	polyPoints2.push_back(Point2<float>(1.12582099, -4.62417889));
	polyPoints2.push_back(Point2<float>(1.12582099, -5.9758215));
	polyPoints2.push_back(Point2<float>(-0.225821018, -5.9758215));

	std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", polyPoints1), CSGPolygon<float>("p2", polyPoints2)};
	std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

	AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
	AssertHelper::assertUnsignedInt(polygonUnion[0].getCwPoints().size(), 4);
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[0], Point2<float>(1.12582099, -5.9758215));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[1], Point2<float>(-0.225821018, -5.9758215));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[2], Point2<float>(-0.225821018, -4.62417889));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[3], Point2<float>(1.12582099, -4.62417889));
}

void PolygonsUnionTest::twoAlmostIdenticalSquares2()
{
	std::vector<Point2<float>> polyPoints1;
	polyPoints1.push_back(Point2<float>(1.0258255, -5.87582397));
	polyPoints1.push_back(Point2<float>(-0.125823975, -5.87582397));
	polyPoints1.push_back(Point2<float>(-0.125823975, -4.72417879));
	polyPoints1.push_back(Point2<float>(1.02582097, -4.72417879));

	std::vector<Point2<float>> polyPoints2;
	polyPoints2.push_back(Point2<float>(1.02582097, -5.87582159));
	polyPoints2.push_back(Point2<float>(-0.125820994, -5.87582159));
	polyPoints2.push_back(Point2<float>(-0.125821024, -4.72417879));
	polyPoints2.push_back(Point2<float>(1.02582097, -4.72417879));

	std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", polyPoints1), CSGPolygon<float>("p2", polyPoints2)};
	std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

	AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
	AssertHelper::assertUnsignedInt(polygonUnion[0].getCwPoints().size(), 4);
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[0], Point2<float>(1.0258255, -5.87582397));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[1], Point2<float>(-0.125823975, -5.87582397));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[2], Point2<float>(-0.125823975, -4.72417879));
	AssertHelper::assertPoint2FloatEquals(polygonUnion[0].getCwPoints()[3], Point2<float>(1.02582097, -4.72417879));
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

	std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", polyPoints1), CSGPolygon<float>("p2", polyPoints2), CSGPolygon<float>("p3", polyPoints3)};
	std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

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

	std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", polyPoints1), CSGPolygon<float>("p2", polyPoints2), CSGPolygon<float>("p3", polyPoints3)};
	std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

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

	suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("onePolygonUnion", &PolygonsUnionTest::onePolygonUnion));

	suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoPolygonsNoUnion", &PolygonsUnionTest::twoPolygonsNoUnion));
	suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("polygonInsideAnother", &PolygonsUnionTest::polygonInsideAnother));
	suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("polygonInsideAnotherTouchInOnePoint", &PolygonsUnionTest::polygonInsideAnotherTouchInOnePoint));

	suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoPolygonsUnion", &PolygonsUnionTest::twoPolygonsUnion));
	suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoPolygonsUnionXAligned", &PolygonsUnionTest::twoPolygonsUnionXAligned));
	suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoPolygonsUnionYAligned", &PolygonsUnionTest::twoPolygonsUnionYAligned));
	suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoPolygonsUnionYAlmostAligned", &PolygonsUnionTest::twoPolygonsUnionYAlmostAligned));
	suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoPolygonsUnionAlmostSameEdge", &PolygonsUnionTest::twoPolygonsUnionAlmostSameEdge));
    suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoPolygonsUnionAlmostSameEdge2", &PolygonsUnionTest::twoPolygonsUnionAlmostSameEdge2));
    suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoPolygonsUnionAlmostSameEdge3", &PolygonsUnionTest::twoPolygonsUnionAlmostSameEdge3));
	suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoPolygonsSameEndPoint", &PolygonsUnionTest::twoPolygonsSameEndPoint));
	suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoPolygonsIntersectionIsEndPoint", &PolygonsUnionTest::twoPolygonsIntersectionIsEndPoint));
	suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoPolygonsIntersectionIsAlmostEndPoint", &PolygonsUnionTest::twoPolygonsIntersectionIsAlmostEndPoint));
	suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoPolygonsIntersectionIsAlmostEdgeA", &PolygonsUnionTest::twoPolygonsIntersectionIsAlmostEdgeA));
	suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoPolygonsTouchInOnePoint", &PolygonsUnionTest::twoPolygonsTouchInOnePoint));
	suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoPolygonsNestedTouchInOnePoint", &PolygonsUnionTest::twoPolygonsNestedTouchInOnePoint));
	suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoPolygonsIntersectionLastEdge", &PolygonsUnionTest::twoPolygonsIntersectionLastEdge));

	suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoIdenticalTriangles", &PolygonsUnionTest::twoIdenticalTriangles));
	suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoIdenticalSquares", &PolygonsUnionTest::twoIdenticalSquares));
	suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoAlmostIdenticalSquares", &PolygonsUnionTest::twoAlmostIdenticalSquares));
	suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoAlmostIdenticalSquares2", &PolygonsUnionTest::twoAlmostIdenticalSquares2));

	suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("threePolygonsUnion", &PolygonsUnionTest::threePolygonsUnion));
	suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoPolygonsUnionAndSeparatePolygon", &PolygonsUnionTest::twoPolygonsUnionAndSeparatePolygon));

	return suite;
}
