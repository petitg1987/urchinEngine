#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <memory>
#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>

#include <AssertHelper.h>
#include <physics/collision/narrowphase/algorithm/gjk/GJKConvexHullTest.h>
#include <physics/collision/narrowphase/algorithm/gjk/GJKTestHelper.h>
using namespace urchin;

void GJKConvexHullTest::identicalBox() {
    Point3<float> obbPointsTab1[] = {
            Point3<float>(-0.3f, 1.0f, 0.0f), Point3<float>(0.2f, 0.0f, 0.0f), Point3<float>(-0.3f, -1.0f, 0.0f), Point3<float>(-0.8f, 0.0f, 0.0f),
            Point3<float>(-0.3f, 1.0f, -1.0f), Point3<float>(0.2f, 0.0f, -1.0f), Point3<float>(-0.3f, -1.0f, -1.0f), Point3<float>(-0.8f, 0.0f, -1.0f)
    };
    std::vector<Point3<float>> obbPoints1(obbPointsTab1, obbPointsTab1+sizeof(obbPointsTab1)/sizeof(Point3<float>));

    Point3<float> obbPointsTab2[] = {
            Point3<float>(-0.3f, 1.0f, 0.0f), Point3<float>(0.2f, 0.0f, 0.0f), Point3<float>(-0.3f, -1.0f, 0.0f), Point3<float>(-0.8f, 0.0f, 0.0f),
            Point3<float>(-0.3f, 1.0f, -1.0f), Point3<float>(0.2f, 0.0f, -1.0f), Point3<float>(-0.3f, -1.0f, -1.0f), Point3<float>(-0.8f, 0.0f, -1.0f)
    };
    std::vector<Point3<float>> obbPoints2(obbPointsTab2, obbPointsTab2+sizeof(obbPointsTab2)/sizeof(Point3<float>));

    CollisionConvexHullObject ch1(0.0f, obbPoints1, obbPoints1);
    CollisionConvexHullObject ch2(0.0f, obbPoints2, obbPoints2);
    std::shared_ptr<GJKResult<float>> result = GJKTestHelper::executeGJK(ch1, ch2);

    AssertHelper::assertTrue(result->isCollide());
}

void GJKConvexHullTest::separateBox() {
    Point3<float> aabbPointsTab[] = {
            Point3<float>(0.0f, 1.0f, 0.0f), Point3<float>(1.0f, 1.0f, 0.0f), Point3<float>(1.0f, -1.0f, 0.0f), Point3<float>(0.0f, -1.0f, 0.0f),
            Point3<float>(0.0f, 1.0f, -1.0f), Point3<float>(1.0f, 1.0f, -1.0f), Point3<float>(1.0f, -1.0f, -1.0f), Point3<float>(0.0f, -1.0f, -1.0f)
    };
    std::vector<Point3<float>> aabbPoints(aabbPointsTab, aabbPointsTab + sizeof(aabbPointsTab) / sizeof(Point3<float>));

    Point3<float> obbPointsTab[] = {
            Point3<float>(-0.6f, 1.0f, 0.0f), Point3<float>(-0.1f, 0.0f, 0.0f), Point3<float>(-0.6f, -1.0f, 0.0f), Point3<float>(-1.1f, 0.0f, 0.0f),
            Point3<float>(-0.6f, 1.0f, -1.0f), Point3<float>(-0.1f, 0.0f, -1.0f), Point3<float>(-0.6f, -1.0f, -1.0f), Point3<float>(-1.1f, 0.0f, -1.0f)
    };
    std::vector<Point3<float>> obbPoints(obbPointsTab, obbPointsTab + sizeof(obbPointsTab) / sizeof(Point3<float>));

    CollisionConvexHullObject ch1(0.0f, aabbPoints, aabbPoints);
    CollisionConvexHullObject ch2(0.0f, obbPoints, obbPoints);
    std::shared_ptr<GJKResult<float>> result = GJKTestHelper::executeGJK(ch1, ch2);

    AssertHelper::assertTrue(!result->isCollide());
    AssertHelper::assertFloatEquals(result->getSeparatingDistance(), 0.1f);
    AssertHelper::assertFloatEquals(result->getClosestPointA().X, 0.0f);
    AssertHelper::assertFloatEquals(result->getClosestPointA().Y, 0.0f);
    AssertHelper::assertFloatEquals(result->getClosestPointB().X, -0.1f);
    AssertHelper::assertFloatEquals(result->getClosestPointB().Y, 0.0f);
    AssertHelper::assertFloatEquals(result->getClosestPointB().vector(result->getClosestPointA()).length(), 0.1f);
}

void GJKConvexHullTest::cornerInsideBox() {
    Point3<float> aabbPointsTab[] = {
            Point3<float>(0.0f, 1.0f, 0.0f), Point3<float>(1.0f, 1.0f, 0.0f), Point3<float>(1.0f, -1.0f, 0.0f), Point3<float>(0.0f, -1.0f, 0.0f),
            Point3<float>(0.0f, 1.0f, -1.0f), Point3<float>(1.0f, 1.0f, -1.0f), Point3<float>(1.0f, -1.0f, -1.0f), Point3<float>(0.0f, -1.0f, -1.0f)
    };
    std::vector<Point3<float>> aabbPoints(aabbPointsTab, aabbPointsTab + sizeof(aabbPointsTab) / sizeof(Point3<float>));

    Point3<float> obbPointsTab[] = {
            Point3<float>(-0.3f, 1.0f, 0.0f), Point3<float>(0.2f, 0.0f, 0.0f), Point3<float>(-0.3f, -1.0f, 0.0f), Point3<float>(-0.8f, 0.0f, 0.0f),
            Point3<float>(-0.3f, 1.0f, -1.0f), Point3<float>(0.2f, 0.0f, -1.0f), Point3<float>(-0.3f, -1.0f, -1.0f), Point3<float>(-0.8f, 0.0f, -1.0f)
    };
    std::vector<Point3<float>> obbPoints(obbPointsTab, obbPointsTab + sizeof(obbPointsTab) / sizeof(Point3<float>));

    CollisionConvexHullObject ch1(0.0f, aabbPoints, aabbPoints);
    CollisionConvexHullObject ch2(0.0f, obbPoints, obbPoints);
    std::shared_ptr<GJKResult<float>> result = GJKTestHelper::executeGJK(ch1, ch2);

    AssertHelper::assertTrue(result->isCollide());
}

void GJKConvexHullTest::faceInsideTrapeze() {
    Point3<float> trapezePointsTab1[] = {
            Point3<float>(1.0f, 0.0f, 0.0f), Point3<float>(3.0f, 0.0f, 0.0f), Point3<float>(0.0f, 3.0f, 0.0f), Point3<float>(0.0f, 1.0f, 0.0f),
            Point3<float>(1.0f, 0.0f, -1.0f), Point3<float>(3.0f, 0.0f, -1.0f), Point3<float>(0.0f, 3.0f, -1.0f), Point3<float>(0.0f, 1.0f, -1.0f),
    };
    std::vector<Point3<float>> trapezePoints1(trapezePointsTab1, trapezePointsTab1+sizeof(trapezePointsTab1)/sizeof(Point3<float>));

    Point3<float> trapezePointsTab2[] = {
            Point3<float>(0.0f, -1.0f, 0.0f), Point3<float>(1.0f, 0.8f, 0.0f), Point3<float>(0.8f, 1.0f, 0.0f), Point3<float>(-1.0f, 0.0f, 0.0f),
            Point3<float>(0.0f, -1.0f, -1.0f), Point3<float>(1.0f, 0.8f, -1.0f), Point3<float>(0.8f, 1.0f, -1.0f), Point3<float>(-1.0f, 0.0f, -1.0f),
    };
    std::vector<Point3<float>> trapezePoints2(trapezePointsTab2, trapezePointsTab2+sizeof(trapezePointsTab2)/sizeof(Point3<float>));

    CollisionConvexHullObject ch1(0.0f, trapezePoints1, trapezePoints1);
    CollisionConvexHullObject ch2(0.0f, trapezePoints2, trapezePoints2);
    std::shared_ptr<GJKResult<float>> result = GJKTestHelper::executeGJK(ch1, ch2);

    AssertHelper::assertTrue(result->isCollide());
}

void GJKConvexHullTest::separateHexagon() {
    Point3<float> hexagonPointsTab1[] = {
            Point3<float>(0.0f, 0.0f, 0.0f), Point3<float>(1.0f, 0.0f, 0.0f), Point3<float>(1.5f, 1.0f, 0.0f), Point3<float>(1.0f, 2.0f, 0.0f), Point3<float>(0.0f, 2.0f, 0.0f), Point3<float>(-0.5f, 1.0f, 0.0f),
            Point3<float>(0.0f, 0.0f, -1.0f), Point3<float>(1.0f, 0.0f, -1.0f), Point3<float>(1.5f, 1.0f, -1.0f), Point3<float>(1.0f, 2.0f, -1.0f), Point3<float>(0.0f, 2.0f, -1.0f), Point3<float>(-0.5f, 1.0f, -1.0f),
    };
    std::vector<Point3<float>> hexagonPoints1(hexagonPointsTab1, hexagonPointsTab1+sizeof(hexagonPointsTab1)/sizeof(Point3<float>));

    Point3<float> hexagonPointsTab2[] = {
            Point3<float>(-2.5f, 0.0f, 0.0f), Point3<float>(-1.5f, 0.0f, 0.0f), Point3<float>(-1.0f, 1.0f, 0.0f), Point3<float>(-1.5f, 2.0f, 0.0f), Point3<float>(-2.5f, 2.0f, 0.0f), Point3<float>(-3.0f, 1.0f, 0.0f),
            Point3<float>(-2.5f, 0.0f, -1.0f), Point3<float>(-1.5f, 0.0f, -1.0f), Point3<float>(-1.0f, 1.0f, -1.0f), Point3<float>(-1.5f, 2.0f, -1.0f), Point3<float>(-2.5f, 2.0f, -1.0f), Point3<float>(-3.0f, 1.0f, -1.0f),
    };
    std::vector<Point3<float>> hexagonPoints2(hexagonPointsTab2, hexagonPointsTab2+sizeof(hexagonPointsTab2)/sizeof(Point3<float>));

    CollisionConvexHullObject ch1(0.0f, hexagonPoints1, hexagonPoints1);
    CollisionConvexHullObject ch2(0.0f, hexagonPoints2, hexagonPoints2);
    std::shared_ptr<GJKResult<float>> result = GJKTestHelper::executeGJK(ch1, ch2);

    AssertHelper::assertTrue(!result->isCollide());
    AssertHelper::assertFloatEquals(result->getSeparatingDistance(), 0.5f);
    AssertHelper::assertFloatEquals(result->getClosestPointA().X, -0.5f);
    AssertHelper::assertFloatEquals(result->getClosestPointA().Y, 1.0f);
    AssertHelper::assertFloatEquals(result->getClosestPointB().X, -1.0f);
    AssertHelper::assertFloatEquals(result->getClosestPointB().Y, 1.0f);
    AssertHelper::assertFloatEquals(result->getClosestPointB().vector(result->getClosestPointA()).length(), 0.5f);
}

void GJKConvexHullTest::cornerInsideHexagon() {
    Point3<float> hexagonPointsTab1[] = {
            Point3<float>(0.0f, 0.0f, 0.0f), Point3<float>(1.0f, 0.0f, 0.0f), Point3<float>(1.5f, 1.0f, 0.0f), Point3<float>(1.0f, 2.0f, 0.0f), Point3<float>(0.0f, 2.0f, 0.0f), Point3<float>(-0.5f, 1.0f, 0.0f),
            Point3<float>(0.0f, 0.0f, -1.0f), Point3<float>(1.0f, 0.0f, -1.0f), Point3<float>(1.5f, 1.0f, -1.0f), Point3<float>(1.0f, 2.0f, -1.0f), Point3<float>(0.0f, 2.0f, -1.0f), Point3<float>(-0.5f, 1.0f, -1.0f),
    };
    std::vector<Point3<float>> hexagonPoints1(hexagonPointsTab1, hexagonPointsTab1+sizeof(hexagonPointsTab1)/sizeof(Point3<float>));

    Point3<float> hexagonPointsTab2[] = {
            Point3<float>(-1.5f, 0.0f, 0.0f), Point3<float>(-0.5f, 0.0f, 0.0f), Point3<float>(0.0f, 1.0f, 0.0f), Point3<float>(-0.5f, 2.0f, 0.0f), Point3<float>(-1.5f, 2.0f, 0.0f), Point3<float>(-2.0f, 1.0f, 0.0f),
            Point3<float>(-1.5f, 0.0f, -1.0f), Point3<float>(-0.5f, 0.0f, -1.0f), Point3<float>(0.0f, 1.0f, -1.0f), Point3<float>(-0.5f, 2.0f, -1.0f), Point3<float>(-1.5f, 2.0f, -1.0f), Point3<float>(-2.0f, 1.0f, -1.0f),
    };
    std::vector<Point3<float>> hexagonPoints2(hexagonPointsTab2, hexagonPointsTab2+sizeof(hexagonPointsTab2)/sizeof(Point3<float>));

    CollisionConvexHullObject ch1(0.0f, hexagonPoints1, hexagonPoints1);
    CollisionConvexHullObject ch2(0.0f, hexagonPoints2, hexagonPoints2);
    std::shared_ptr<GJKResult<float>> result = GJKTestHelper::executeGJK(ch1, ch2);

    AssertHelper::assertTrue(result->isCollide());
}

CppUnit::Test* GJKConvexHullTest::suite() {
    auto* suite = new CppUnit::TestSuite("GJKConvexHullTest");

    suite->addTest(new CppUnit::TestCaller<GJKConvexHullTest>("identicalBox", &GJKConvexHullTest::identicalBox));
    suite->addTest(new CppUnit::TestCaller<GJKConvexHullTest>("separateBox", &GJKConvexHullTest::separateBox));
    suite->addTest(new CppUnit::TestCaller<GJKConvexHullTest>("cornerInsideBox", &GJKConvexHullTest::cornerInsideBox));

    suite->addTest(new CppUnit::TestCaller<GJKConvexHullTest>("faceInsideTrapeze", &GJKConvexHullTest::faceInsideTrapeze));

    suite->addTest(new CppUnit::TestCaller<GJKConvexHullTest>("separateHexagon", &GJKConvexHullTest::separateHexagon));
    suite->addTest(new CppUnit::TestCaller<GJKConvexHullTest>("cornerInsideHexagon", &GJKConvexHullTest::cornerInsideHexagon));

    return suite;
}
