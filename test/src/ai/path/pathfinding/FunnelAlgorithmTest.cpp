#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <UrchinCommon.h>
#include <UrchinAIEngine.h>

#include <ai/path/pathfinding/FunnelAlgorithmTest.h>
#include <AssertHelper.h>
using namespace urchin;

void FunnelAlgorithmTest::straightPath() {
    std::vector<std::unique_ptr<PathPortal>> portals;
    portals.push_back(std::make_unique<PathPortal>(LineSegment3D(Point3(1.0f, 0.0f, -1.0f), Point3(1.0f, 0.0f, -1.0f)), nullptr, nullptr, false)); //start point
    portals.push_back(std::make_unique<PathPortal>(LineSegment3D(Point3(2.0f, 0.0f, 0.0f), Point3(0.0f, 0.0f, 0.0f)), nullptr, nullptr, false));
    portals.push_back(std::make_unique<PathPortal>(LineSegment3D(Point3(1.0f, 0.0f, 1.0f), Point3(1.0f, 0.0f, 1.0f)), nullptr, nullptr, false)); //end point

    FunnelAlgorithm().computePivotPoints(portals);

    AssertHelper::assertPoint3FloatEquals(portals[0]->getTransitionPoint(), Point3(1.0f, 0.0f, -1.0f));
    AssertHelper::assertFalse(portals[1]->hasTransitionPoint());
    AssertHelper::assertPoint3FloatEquals(portals[2]->getTransitionPoint(), Point3(1.0f, 0.0f, 1.0f));
}

void FunnelAlgorithmTest::cornerPath1() {
    std::vector<std::unique_ptr<PathPortal>> portals;
    portals.push_back(std::make_unique<PathPortal>(LineSegment3D(Point3(1.0f, 0.0f, -1.0f), Point3(1.0f, 0.0f, -1.0f)), nullptr, nullptr, false)); //start point
    portals.push_back(std::make_unique<PathPortal>(LineSegment3D(Point3(2.0f, 0.0f, 0.0f), Point3(0.0f, 0.0f, 0.0f)), nullptr, nullptr, false));
    portals.push_back(std::make_unique<PathPortal>(LineSegment3D(Point3(4.0f, 0.0f, 1.0f), Point3(4.0f, 0.0f, 1.0f)), nullptr, nullptr, false)); //end point

    FunnelAlgorithm().computePivotPoints(portals);

    AssertHelper::assertPoint3FloatEquals(portals[0]->getTransitionPoint(), Point3(1.0f, 0.0f, -1.0f));
    AssertHelper::assertPoint3FloatEquals(portals[1]->getTransitionPoint(), Point3(2.0f, 0.0f, 0.0f));
    AssertHelper::assertPoint3FloatEquals(portals[2]->getTransitionPoint(), Point3(4.0f, 0.0f, 1.0f));
}

void FunnelAlgorithmTest::cornerPath2() {
    std::vector<std::unique_ptr<PathPortal>> portals;
    portals.push_back(std::make_unique<PathPortal>(LineSegment3D(Point3(4.0f, 0.0f, 1.0f), Point3(4.0f, 0.0f, 1.0f)), nullptr, nullptr, false)); //start point
    portals.push_back(std::make_unique<PathPortal>(LineSegment3D(Point3(0.0f, 0.0f, 0.0f), Point3(2.0f, 0.0f, 0.0f)), nullptr, nullptr, false));
    portals.push_back(std::make_unique<PathPortal>(LineSegment3D(Point3(1.0f, 0.0f, -1.0f), Point3(1.0f, 0.0f, -1.0f)), nullptr, nullptr, false)); //end point

    FunnelAlgorithm().computePivotPoints(portals);

    AssertHelper::assertPoint3FloatEquals(portals[0]->getTransitionPoint(), Point3(4.0f, 0.0f, 1.0f));
    AssertHelper::assertPoint3FloatEquals(portals[1]->getTransitionPoint(), Point3(2.0f, 0.0f, 0.0f));
    AssertHelper::assertPoint3FloatEquals(portals[2]->getTransitionPoint(), Point3(1.0f, 0.0f, -1.0f));
}

void FunnelAlgorithmTest::cornerPath3() {
    std::vector<std::unique_ptr<PathPortal>> portals;
    portals.push_back(std::make_unique<PathPortal>(LineSegment3D(Point3(1.0f, 0.0f, -1.0f), Point3(1.0f, 0.0f, -1.0f)), nullptr, nullptr, false)); //start point
    portals.push_back(std::make_unique<PathPortal>(LineSegment3D(Point3(2.0f, 0.0f, 0.0f), Point3(0.0f, 0.0f, 0.0f)), nullptr, nullptr, false));
    portals.push_back(std::make_unique<PathPortal>(LineSegment3D(Point3(-2.0f, 0.0f, 1.0f), Point3(-2.0f, 0.0f, 1.0f)), nullptr, nullptr, false)); //end point

    FunnelAlgorithm().computePivotPoints(portals);

    AssertHelper::assertPoint3FloatEquals(portals[0]->getTransitionPoint(), Point3(1.0f, 0.0f, -1.0f));
    AssertHelper::assertPoint3FloatEquals(portals[1]->getTransitionPoint(), Point3(0.0f, 0.0f, 0.0f));
    AssertHelper::assertPoint3FloatEquals(portals[2]->getTransitionPoint(), Point3(-2.0f, 0.0f, 1.0f));
}

void FunnelAlgorithmTest::cornerPath4() {
    std::vector<std::unique_ptr<PathPortal>> portals;
    portals.push_back(std::make_unique<PathPortal>(LineSegment3D(Point3(-2.0f, 0.0f, 1.0f), Point3(-2.0f, 0.0f, 1.0f)), nullptr, nullptr, false)); //end point
    portals.push_back(std::make_unique<PathPortal>(LineSegment3D(Point3(0.0f, 0.0f, 0.0f), Point3(2.0f, 0.0f, 0.0f)), nullptr, nullptr, false));
    portals.push_back(std::make_unique<PathPortal>(LineSegment3D(Point3(1.0f, 0.0f, -1.0f), Point3(1.0f, 0.0f, -1.0f)), nullptr, nullptr, false)); //start point

    FunnelAlgorithm().computePivotPoints(portals);

    AssertHelper::assertPoint3FloatEquals(portals[0]->getTransitionPoint(), Point3(-2.0f, 0.0f, 1.0f));
    AssertHelper::assertPoint3FloatEquals(portals[1]->getTransitionPoint(), Point3(0.0f, 0.0f, 0.0f));
    AssertHelper::assertPoint3FloatEquals(portals[2]->getTransitionPoint(), Point3(1.0f, 0.0f, -1.0f));
}

CppUnit::Test* FunnelAlgorithmTest::suite() {
    auto* suite = new CppUnit::TestSuite("FunnelAlgorithmTest");

    suite->addTest(new CppUnit::TestCaller("straightPath", &FunnelAlgorithmTest::straightPath));

    suite->addTest(new CppUnit::TestCaller("cornerPath1", &FunnelAlgorithmTest::cornerPath1));
    suite->addTest(new CppUnit::TestCaller("cornerPath2", &FunnelAlgorithmTest::cornerPath2));
    suite->addTest(new CppUnit::TestCaller("cornerPath3", &FunnelAlgorithmTest::cornerPath3));
    suite->addTest(new CppUnit::TestCaller("cornerPath4", &FunnelAlgorithmTest::cornerPath4));

    return suite;
}
