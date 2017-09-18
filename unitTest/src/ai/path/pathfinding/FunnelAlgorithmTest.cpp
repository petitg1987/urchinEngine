#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include "UrchinCommon.h"
#include "UrchinAIEngine.h"

#include "FunnelAlgorithmTest.h"
#include "AssertHelper.h"

void FunnelAlgorithmTest::straightPath()
{
    auto portals = std::make_shared<std::vector<LineSegment3D<float>>>();
    portals->push_back(LineSegment3D<float>(Point3<float>(1.0, 0.0, 1.0), Point3<float>(1.0, 0.0, 1.0))); //start point
    portals->push_back(LineSegment3D<float>(Point3<float>(0.0, 0.0, 0.0), Point3<float>(2.0, 0.0, 0.0)));
    portals->push_back(LineSegment3D<float>(Point3<float>(1.0, 0.0, -1.0), Point3<float>(1.0, 0.0, -1.0))); //end point

    std::vector<Point3<float>> path = FunnelAlgorithm(portals).findPath();

    AssertHelper::assertUnsignedInt(path.size(), 2);
    AssertHelper::assertPoint3FloatEquals(path[0], Point3<float>(1.0, 0.0, 1.0));
    AssertHelper::assertPoint3FloatEquals(path[1], Point3<float>(1.0, 0.0, -1.0));
}

void FunnelAlgorithmTest::rightCornerPath()
{
    auto portals = std::make_shared<std::vector<LineSegment3D<float>>>();
    portals->push_back(LineSegment3D<float>(Point3<float>(1.0, 0.0, 1.0), Point3<float>(1.0, 0.0, 1.0))); //start point
    portals->push_back(LineSegment3D<float>(Point3<float>(0.0, 0.0, 0.0), Point3<float>(2.0, 0.0, 0.0)));
    portals->push_back(LineSegment3D<float>(Point3<float>(4.0, 0.0, -1.0), Point3<float>(4.0, 0.0, -1.0))); //end point

    std::vector<Point3<float>> path = FunnelAlgorithm(portals).findPath();

    AssertHelper::assertUnsignedInt(path.size(), 3);
    AssertHelper::assertPoint3FloatEquals(path[0], Point3<float>(1.0, 0.0, 1.0));
    AssertHelper::assertPoint3FloatEquals(path[1], Point3<float>(2.0, 0.0, 0.0));
    AssertHelper::assertPoint3FloatEquals(path[2], Point3<float>(4.0, 0.0, -1.0));
}

void FunnelAlgorithmTest::leftCornerPath()
{
    auto portals = std::make_shared<std::vector<LineSegment3D<float>>>();
    portals->push_back(LineSegment3D<float>(Point3<float>(1.0, 0.0, 1.0), Point3<float>(1.0, 0.0, 1.0))); //start point
    portals->push_back(LineSegment3D<float>(Point3<float>(0.0, 0.0, 0.0), Point3<float>(2.0, 0.0, 0.0)));
    portals->push_back(LineSegment3D<float>(Point3<float>(-2.0, 0.0, -1.0), Point3<float>(-2.0, 0.0, -1.0))); //end point

    std::vector<Point3<float>> path = FunnelAlgorithm(portals).findPath();

    AssertHelper::assertUnsignedInt(path.size(), 3);
    AssertHelper::assertPoint3FloatEquals(path[0], Point3<float>(1.0, 0.0, 1.0));
    AssertHelper::assertPoint3FloatEquals(path[1], Point3<float>(0.0, 0.0, 0.0));
    AssertHelper::assertPoint3FloatEquals(path[2], Point3<float>(-2.0, 0.0, -1.0));
}

CppUnit::Test *FunnelAlgorithmTest::suite()
{
    CppUnit::TestSuite *suite = new CppUnit::TestSuite("FunnelAlgorithmTest");

    suite->addTest(new CppUnit::TestCaller<FunnelAlgorithmTest>("straightPath", &FunnelAlgorithmTest::straightPath));
    suite->addTest(new CppUnit::TestCaller<FunnelAlgorithmTest>("rightCornerPath", &FunnelAlgorithmTest::rightCornerPath));
    suite->addTest(new CppUnit::TestCaller<FunnelAlgorithmTest>("leftCornerPath", &FunnelAlgorithmTest::leftCornerPath));

    return suite;
}
