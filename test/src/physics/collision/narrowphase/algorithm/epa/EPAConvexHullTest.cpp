#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <memory>
#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>

#include <AssertHelper.h>
#include <physics/collision/narrowphase/algorithm/epa/EPAConvexHullTest.h>
#include <physics/collision/narrowphase/algorithm/epa/EPATestHelper.h>
using namespace urchin;

void EPAConvexHullTest::identicalBox() {
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
    std::shared_ptr<EPAResult<float>> resultEpa = EPATestHelper::executeEPA(ch1, ch2);

    AssertHelper::assertTrue(resultEpa->isCollide());
    AssertHelper::assertFloatEquals(resultEpa->getPenetrationDepth(), 0.894427191f);
    AssertHelper::assertFloatEquals(resultEpa->getContactPointA().vector(resultEpa->getContactPointB()).length(), 0.894427191f);
}

void EPAConvexHullTest::cornerInsideBox() {
    Point3<float> aabbPointsTab[] = {
            Point3<float>(0.0f, 1.0f, 0.0f), Point3<float>(1.0f, 1.0f, 0.0f), Point3<float>(1.0f, -1.0f, 0.0f), Point3<float>(0.0f, -1.0f, 0.0f),
            Point3<float>(0.0f, 1.0f, -1.0f), Point3<float>(1.0f, 1.0f, -1.0f), Point3<float>(1.0f, -1.0f, -1.0f), Point3<float>(0.0f, -1.0f, -1.0f)
    };
    std::vector<Point3<float>> aabbPoints(aabbPointsTab, aabbPointsTab+sizeof(aabbPointsTab)/sizeof(Point3<float>));

    Point3<float> obbPointsTab[] = {
            Point3<float>(-0.3f, 1.0f, 0.0f), Point3<float>(0.2f, 0.0f, 0.0f), Point3<float>(-0.3f, -1.0f, 0.0f), Point3<float>(-0.8f, 0.0f, 0.0f),
            Point3<float>(-0.3f, 1.0f, -1.0f), Point3<float>(0.2f, 0.0f, -1.0f), Point3<float>(-0.3f, -1.0f, -1.0f), Point3<float>(-0.8f, 0.0f, -1.0f)
    };
    std::vector<Point3<float>> obbPoints(obbPointsTab, obbPointsTab+sizeof(obbPointsTab)/sizeof(Point3<float>));

    CollisionConvexHullObject ch1(0.0f, aabbPoints, aabbPoints);
    CollisionConvexHullObject ch2(0.0f, obbPoints, obbPoints);
    std::shared_ptr<EPAResult<float>> resultEpa = EPATestHelper::executeEPA(ch1, ch2);

    AssertHelper::assertTrue(resultEpa->isCollide());
    AssertHelper::assertFloatEquals(resultEpa->getPenetrationDepth(), 0.2f);
    AssertHelper::assertVector3FloatEquals(resultEpa->getNormal(), Vector3<float>(-1.0f, 0.0f, 0.0f));
    AssertHelper::assertFloatEquals(resultEpa->getContactPointA().vector(resultEpa->getContactPointB()).length(), 0.2f);
    AssertHelper::assertFloatEquals(resultEpa->getContactPointA().X, 0.0f);
    AssertHelper::assertFloatEquals(resultEpa->getContactPointA().Y, 0.0f);
    AssertHelper::assertFloatEquals(resultEpa->getContactPointB().X, 0.2f);
    AssertHelper::assertFloatEquals(resultEpa->getContactPointB().Y, 0.0f);
    AssertHelper::assertFloatEquals(resultEpa->getContactPointA().Z, resultEpa->getContactPointB().Z);
}

void EPAConvexHullTest::faceInsideTrapeze() {
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
    std::shared_ptr<EPAResult<float>> resultEpa = EPATestHelper::executeEPA(ch1, ch2);

    AssertHelper::assertTrue(resultEpa->isCollide());
    AssertHelper::assertFloatEquals(resultEpa->getPenetrationDepth(), 0.56568542494f);
    AssertHelper::assertVector3FloatEquals(resultEpa->getNormal(), Vector3<float>(-0.70710678118f, -0.70710678118f, 0.0f));
    AssertHelper::assertFloatEquals(resultEpa->getContactPointA().vector(resultEpa->getContactPointB()).length(), 0.56568542494f);
    AssertHelper::assertFloatEquals(resultEpa->getContactPointA().X, 0.5f);
    AssertHelper::assertFloatEquals(resultEpa->getContactPointA().Y, 0.5f);
    AssertHelper::assertFloatEquals(resultEpa->getContactPointB().X, 0.9f);
    AssertHelper::assertFloatEquals(resultEpa->getContactPointB().Y, 0.9f);
    AssertHelper::assertFloatEquals(resultEpa->getContactPointA().Z, resultEpa->getContactPointB().Z);
}

void EPAConvexHullTest::cornerInsideHexagon() {
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
    std::shared_ptr<EPAResult<float>> resultEpa = EPATestHelper::executeEPA(ch1, ch2);

    AssertHelper::assertTrue(resultEpa->isCollide());
    AssertHelper::assertFloatEquals(resultEpa->getPenetrationDepth(), 0.44721359f);
    AssertHelper::assertFloatEquals(resultEpa->getContactPointA().vector(resultEpa->getContactPointB()).length(), 0.44721359f);
    AssertHelper::assertFloatEquals(resultEpa->getContactPointA().Z, resultEpa->getContactPointB().Z);
}

CppUnit::Test* EPAConvexHullTest::suite() {
    auto* suite = new CppUnit::TestSuite("EPAConvexHullTest");

    suite->addTest(new CppUnit::TestCaller<EPAConvexHullTest>("identicalBox", &EPAConvexHullTest::identicalBox));
    suite->addTest(new CppUnit::TestCaller<EPAConvexHullTest>("cornerInsideBox", &EPAConvexHullTest::cornerInsideBox));

    suite->addTest(new CppUnit::TestCaller<EPAConvexHullTest>("faceInsideTrapeze", &EPAConvexHullTest::faceInsideTrapeze));

    suite->addTest(new CppUnit::TestCaller<EPAConvexHullTest>("cornerInsideHexagon", &EPAConvexHullTest::cornerInsideHexagon));

    return suite;
}
