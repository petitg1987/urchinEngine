#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include "UrchinCommon.h"
#include "UrchinAIEngine.h"

#include "FunnelAlgorithmTest.h"
#include "AssertHelper.h"

void FunnelAlgorithmTest::straightPath()
{
    std::vector<std::shared_ptr<PathPortal>> portals;
    portals.push_back(std::make_shared<PathPortal>(LineSegment3D<float>(Point3<float>(1.0, 0.0, -1.0), Point3<float>(1.0, 0.0, -1.0)), nullptr, nullptr)); //start point
    portals.push_back(std::make_shared<PathPortal>(LineSegment3D<float>(Point3<float>(2.0, 0.0, 0.0), Point3<float>(0.0, 0.0, 0.0)), nullptr, nullptr));
    portals.push_back(std::make_shared<PathPortal>(LineSegment3D<float>(Point3<float>(1.0, 0.0, 1.0), Point3<float>(1.0, 0.0, 1.0)), nullptr, nullptr)); //end point

    std::vector<Point3<float>> path = FunnelAlgorithm(portals).findPath();

    AssertHelper::assertUnsignedInt(path.size(), 2);
    AssertHelper::assertPoint3FloatEquals(path[0], Point3<float>(1.0, 0.0, -1.0));
    AssertHelper::assertPoint3FloatEquals(path[1], Point3<float>(1.0, 0.0, 1.0));
}

void FunnelAlgorithmTest::cornerPath1()
{
    std::vector<std::shared_ptr<PathPortal>> portals;
    portals.push_back(std::make_shared<PathPortal>(LineSegment3D<float>(Point3<float>(1.0, 0.0, -1.0), Point3<float>(1.0, 0.0, -1.0)), nullptr, nullptr)); //start point
    portals.push_back(std::make_shared<PathPortal>(LineSegment3D<float>(Point3<float>(2.0, 0.0, 0.0), Point3<float>(0.0, 0.0, 0.0)), nullptr, nullptr));
    portals.push_back(std::make_shared<PathPortal>(LineSegment3D<float>(Point3<float>(4.0, 0.0, 1.0), Point3<float>(4.0, 0.0, 1.0)), nullptr, nullptr)); //end point

    std::vector<Point3<float>> path = FunnelAlgorithm(portals).findPath();

    AssertHelper::assertUnsignedInt(path.size(), 3);
    AssertHelper::assertPoint3FloatEquals(path[0], Point3<float>(1.0, 0.0, -1.0));
    AssertHelper::assertPoint3FloatEquals(path[1], Point3<float>(2.0, 0.0, 0.0));
    AssertHelper::assertPoint3FloatEquals(path[2], Point3<float>(4.0, 0.0, 1.0));
}

void FunnelAlgorithmTest::cornerPath2()
{
    std::vector<std::shared_ptr<PathPortal>> portals;
    portals.push_back(std::make_shared<PathPortal>(LineSegment3D<float>(Point3<float>(4.0, 0.0, 1.0), Point3<float>(4.0, 0.0, 1.0)), nullptr, nullptr)); //start point
    portals.push_back(std::make_shared<PathPortal>(LineSegment3D<float>(Point3<float>(0.0, 0.0, 0.0), Point3<float>(2.0, 0.0, 0.0)), nullptr, nullptr));
    portals.push_back(std::make_shared<PathPortal>(LineSegment3D<float>(Point3<float>(1.0, 0.0, -1.0), Point3<float>(1.0, 0.0, -1.0)), nullptr, nullptr)); //end point

    std::vector<Point3<float>> path = FunnelAlgorithm(portals).findPath();

    AssertHelper::assertUnsignedInt(path.size(), 3);
    AssertHelper::assertPoint3FloatEquals(path[0], Point3<float>(4.0, 0.0, 1.0));
    AssertHelper::assertPoint3FloatEquals(path[1], Point3<float>(2.0, 0.0, 0.0));
    AssertHelper::assertPoint3FloatEquals(path[2], Point3<float>(1.0, 0.0, -1.0));
}


void FunnelAlgorithmTest::cornerPath3()
{
    std::vector<std::shared_ptr<PathPortal>> portals;
    portals.push_back(std::make_shared<PathPortal>(LineSegment3D<float>(Point3<float>(1.0, 0.0, -1.0), Point3<float>(1.0, 0.0, -1.0)), nullptr, nullptr)); //start point
    portals.push_back(std::make_shared<PathPortal>(LineSegment3D<float>(Point3<float>(2.0, 0.0, 0.0), Point3<float>(0.0, 0.0, 0.0)), nullptr, nullptr));
    portals.push_back(std::make_shared<PathPortal>(LineSegment3D<float>(Point3<float>(-2.0, 0.0, 1.0), Point3<float>(-2.0, 0.0, 1.0)), nullptr, nullptr)); //end point

    std::vector<Point3<float>> path = FunnelAlgorithm(portals).findPath();

    AssertHelper::assertUnsignedInt(path.size(), 3);
    AssertHelper::assertPoint3FloatEquals(path[0], Point3<float>(1.0, 0.0, -1.0));
    AssertHelper::assertPoint3FloatEquals(path[1], Point3<float>(0.0, 0.0, 0.0));
    AssertHelper::assertPoint3FloatEquals(path[2], Point3<float>(-2.0, 0.0, 1.0));
}

void FunnelAlgorithmTest::cornerPath4()
{
    std::vector<std::shared_ptr<PathPortal>> portals;
    portals.push_back(std::make_shared<PathPortal>(LineSegment3D<float>(Point3<float>(-2.0, 0.0, 1.0), Point3<float>(-2.0, 0.0, 1.0)), nullptr, nullptr)); //end point
    portals.push_back(std::make_shared<PathPortal>(LineSegment3D<float>(Point3<float>(0.0, 0.0, 0.0), Point3<float>(2.0, 0.0, 0.0)), nullptr, nullptr));
    portals.push_back(std::make_shared<PathPortal>(LineSegment3D<float>(Point3<float>(1.0, 0.0, -1.0), Point3<float>(1.0, 0.0, -1.0)), nullptr, nullptr)); //start point

    std::vector<Point3<float>> path = FunnelAlgorithm(portals).findPath();

    AssertHelper::assertUnsignedInt(path.size(), 3);
    AssertHelper::assertPoint3FloatEquals(path[0], Point3<float>(-2.0, 0.0, 1.0));
    AssertHelper::assertPoint3FloatEquals(path[1], Point3<float>(0.0, 0.0, 0.0));
    AssertHelper::assertPoint3FloatEquals(path[2], Point3<float>(1.0, 0.0, -1.0));
}

CppUnit::Test *FunnelAlgorithmTest::suite()
{
    CppUnit::TestSuite *suite = new CppUnit::TestSuite("FunnelAlgorithmTest");

    suite->addTest(new CppUnit::TestCaller<FunnelAlgorithmTest>("straightPath", &FunnelAlgorithmTest::straightPath));

    suite->addTest(new CppUnit::TestCaller<FunnelAlgorithmTest>("cornerPath1", &FunnelAlgorithmTest::cornerPath1));
    suite->addTest(new CppUnit::TestCaller<FunnelAlgorithmTest>("cornerPath2", &FunnelAlgorithmTest::cornerPath2));
    suite->addTest(new CppUnit::TestCaller<FunnelAlgorithmTest>("cornerPath3", &FunnelAlgorithmTest::cornerPath3));
    suite->addTest(new CppUnit::TestCaller<FunnelAlgorithmTest>("cornerPath4", &FunnelAlgorithmTest::cornerPath4));

    return suite;
}
