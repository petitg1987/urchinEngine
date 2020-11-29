#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include "UrchinCommon.h"
#include "UrchinAIEngine.h"

#include "FunnelAlgorithmTest.h"
#include "AssertHelper.h"
using namespace urchin;

void FunnelAlgorithmTest::straightPath() {
    std::vector<std::shared_ptr<PathPortal>> portals;
    portals.push_back(std::make_shared<PathPortal>(LineSegment3D<float>(Point3<float>(1.0, 0.0, -1.0), Point3<float>(1.0, 0.0, -1.0)), nullptr, nullptr, false)); //start point
    portals.push_back(std::make_shared<PathPortal>(LineSegment3D<float>(Point3<float>(2.0, 0.0, 0.0), Point3<float>(0.0, 0.0, 0.0)), nullptr, nullptr, false));
    portals.push_back(std::make_shared<PathPortal>(LineSegment3D<float>(Point3<float>(1.0, 0.0, 1.0), Point3<float>(1.0, 0.0, 1.0)), nullptr, nullptr, false)); //end point

    std::vector<std::shared_ptr<PathPortal>> pathPortals = FunnelAlgorithm(portals).computePivotPoints();

    AssertHelper::assertPoint3FloatEquals(pathPortals[0]->getTransitionPoint(), Point3<float>(1.0, 0.0, -1.0));
    AssertHelper::assertTrue(!pathPortals[1]->hasTransitionPoint());
    AssertHelper::assertPoint3FloatEquals(pathPortals[2]->getTransitionPoint(), Point3<float>(1.0, 0.0, 1.0));
}

void FunnelAlgorithmTest::cornerPath1() {
    std::vector<std::shared_ptr<PathPortal>> portals;
    portals.push_back(std::make_shared<PathPortal>(LineSegment3D<float>(Point3<float>(1.0, 0.0, -1.0), Point3<float>(1.0, 0.0, -1.0)), nullptr, nullptr, false)); //start point
    portals.push_back(std::make_shared<PathPortal>(LineSegment3D<float>(Point3<float>(2.0, 0.0, 0.0), Point3<float>(0.0, 0.0, 0.0)), nullptr, nullptr, false));
    portals.push_back(std::make_shared<PathPortal>(LineSegment3D<float>(Point3<float>(4.0, 0.0, 1.0), Point3<float>(4.0, 0.0, 1.0)), nullptr, nullptr, false)); //end point

    std::vector<std::shared_ptr<PathPortal>> pathPortals = FunnelAlgorithm(portals).computePivotPoints();

    AssertHelper::assertPoint3FloatEquals(pathPortals[0]->getTransitionPoint(), Point3<float>(1.0, 0.0, -1.0));
    AssertHelper::assertPoint3FloatEquals(pathPortals[1]->getTransitionPoint(), Point3<float>(2.0, 0.0, 0.0));
    AssertHelper::assertPoint3FloatEquals(pathPortals[2]->getTransitionPoint(), Point3<float>(4.0, 0.0, 1.0));
}

void FunnelAlgorithmTest::cornerPath2() {
    std::vector<std::shared_ptr<PathPortal>> portals;
    portals.push_back(std::make_shared<PathPortal>(LineSegment3D<float>(Point3<float>(4.0, 0.0, 1.0), Point3<float>(4.0, 0.0, 1.0)), nullptr, nullptr, false)); //start point
    portals.push_back(std::make_shared<PathPortal>(LineSegment3D<float>(Point3<float>(0.0, 0.0, 0.0), Point3<float>(2.0, 0.0, 0.0)), nullptr, nullptr, false));
    portals.push_back(std::make_shared<PathPortal>(LineSegment3D<float>(Point3<float>(1.0, 0.0, -1.0), Point3<float>(1.0, 0.0, -1.0)), nullptr, nullptr, false)); //end point

    std::vector<std::shared_ptr<PathPortal>> pathPortals = FunnelAlgorithm(portals).computePivotPoints();

    AssertHelper::assertPoint3FloatEquals(pathPortals[0]->getTransitionPoint(), Point3<float>(4.0, 0.0, 1.0));
    AssertHelper::assertPoint3FloatEquals(pathPortals[1]->getTransitionPoint(), Point3<float>(2.0, 0.0, 0.0));
    AssertHelper::assertPoint3FloatEquals(pathPortals[2]->getTransitionPoint(), Point3<float>(1.0, 0.0, -1.0));
}


void FunnelAlgorithmTest::cornerPath3() {
    std::vector<std::shared_ptr<PathPortal>> portals;
    portals.push_back(std::make_shared<PathPortal>(LineSegment3D<float>(Point3<float>(1.0, 0.0, -1.0), Point3<float>(1.0, 0.0, -1.0)), nullptr, nullptr, false)); //start point
    portals.push_back(std::make_shared<PathPortal>(LineSegment3D<float>(Point3<float>(2.0, 0.0, 0.0), Point3<float>(0.0, 0.0, 0.0)), nullptr, nullptr, false));
    portals.push_back(std::make_shared<PathPortal>(LineSegment3D<float>(Point3<float>(-2.0, 0.0, 1.0), Point3<float>(-2.0, 0.0, 1.0)), nullptr, nullptr, false)); //end point

    std::vector<std::shared_ptr<PathPortal>> pathPortals = FunnelAlgorithm(portals).computePivotPoints();

    AssertHelper::assertPoint3FloatEquals(pathPortals[0]->getTransitionPoint(), Point3<float>(1.0, 0.0, -1.0));
    AssertHelper::assertPoint3FloatEquals(pathPortals[1]->getTransitionPoint(), Point3<float>(0.0, 0.0, 0.0));
    AssertHelper::assertPoint3FloatEquals(pathPortals[2]->getTransitionPoint(), Point3<float>(-2.0, 0.0, 1.0));
}

void FunnelAlgorithmTest::cornerPath4() {
    std::vector<std::shared_ptr<PathPortal>> portals;
    portals.push_back(std::make_shared<PathPortal>(LineSegment3D<float>(Point3<float>(-2.0, 0.0, 1.0), Point3<float>(-2.0, 0.0, 1.0)), nullptr, nullptr, false)); //end point
    portals.push_back(std::make_shared<PathPortal>(LineSegment3D<float>(Point3<float>(0.0, 0.0, 0.0), Point3<float>(2.0, 0.0, 0.0)), nullptr, nullptr, false));
    portals.push_back(std::make_shared<PathPortal>(LineSegment3D<float>(Point3<float>(1.0, 0.0, -1.0), Point3<float>(1.0, 0.0, -1.0)), nullptr, nullptr, false)); //start point

    std::vector<std::shared_ptr<PathPortal>> pathPortals = FunnelAlgorithm(portals).computePivotPoints();

    AssertHelper::assertPoint3FloatEquals(pathPortals[0]->getTransitionPoint(), Point3<float>(-2.0, 0.0, 1.0));
    AssertHelper::assertPoint3FloatEquals(pathPortals[1]->getTransitionPoint(), Point3<float>(0.0, 0.0, 0.0));
    AssertHelper::assertPoint3FloatEquals(pathPortals[2]->getTransitionPoint(), Point3<float>(1.0, 0.0, -1.0));
}

CppUnit::Test *FunnelAlgorithmTest::suite() {
    auto* suite = new CppUnit::TestSuite("FunnelAlgorithmTest");

    suite->addTest(new CppUnit::TestCaller<FunnelAlgorithmTest>("straightPath", &FunnelAlgorithmTest::straightPath));

    suite->addTest(new CppUnit::TestCaller<FunnelAlgorithmTest>("cornerPath1", &FunnelAlgorithmTest::cornerPath1));
    suite->addTest(new CppUnit::TestCaller<FunnelAlgorithmTest>("cornerPath2", &FunnelAlgorithmTest::cornerPath2));
    suite->addTest(new CppUnit::TestCaller<FunnelAlgorithmTest>("cornerPath3", &FunnelAlgorithmTest::cornerPath3));
    suite->addTest(new CppUnit::TestCaller<FunnelAlgorithmTest>("cornerPath4", &FunnelAlgorithmTest::cornerPath4));

    return suite;
}
