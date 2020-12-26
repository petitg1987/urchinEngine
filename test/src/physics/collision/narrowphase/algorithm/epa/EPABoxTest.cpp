#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <memory>
#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"

#include "AssertHelper.h"
#include "physics/collision/narrowphase/algorithm/epa/EPABoxTest.h"
#include "physics/collision/narrowphase/algorithm/epa/EPATestHelper.h"
using namespace urchin;

void EPABoxTest::identicalAABBox() {
    CollisionBoxObject aabbox1(0.0f, Vector3<float>(0.5f, 0.5f, 0.5f), Point3<float>(0.5f, 0.5f, 0.5f), Quaternion<float>());
    CollisionBoxObject aabbox2(0.0f, Vector3<float>(0.5f, 0.5f, 0.5f), Point3<float>(0.5f, 0.5f, 0.5f), Quaternion<float>());

    std::shared_ptr<EPAResult<float>> resultEpa = EPATestHelper::executeEPA(aabbox1, aabbox2);

    AssertHelper::instance()->assertTrue(resultEpa->isCollide());
    AssertHelper::instance()->assertFloatEquals(resultEpa->getPenetrationDepth(), 1.0f);
    AssertHelper::instance()->assertFloatEquals(resultEpa->getContactPointA().vector(resultEpa->getContactPointB()).length(), 1.0f);
}

void EPABoxTest::overlapAABBox() {
    CollisionBoxObject aabbox1(0.0f, Vector3<float>(0.5f, 0.5f, 0.5f), Point3<float>(0.5f, 0.5f, 0.5f), Quaternion<float>());
    CollisionBoxObject aabbox2(0.0f, Vector3<float>(0.5f, 0.5f, 0.5f), Point3<float>(1.1f, 0.5f, 0.5f), Quaternion<float>());

    std::shared_ptr<EPAResult<float>> resultEpa = EPATestHelper::executeEPA(aabbox1, aabbox2);

    AssertHelper::instance()->assertTrue(resultEpa->isCollide());
    AssertHelper::instance()->assertFloatEquals(resultEpa->getPenetrationDepth(), 0.4f);
    AssertHelper::instance()->assertVector3FloatEquals(resultEpa->getNormal(), Vector3<float>(1.0f, 0.0f, 0.0f));
    AssertHelper::instance()->assertFloatEquals(resultEpa->getContactPointA().vector(resultEpa->getContactPointB()).length(), 0.4f);
    AssertHelper::instance()->assertFloatEquals(resultEpa->getContactPointA().X, 1.0f);
    AssertHelper::instance()->assertFloatEquals(resultEpa->getContactPointB().X, 0.6f);
    AssertHelper::instance()->assertFloatEquals(resultEpa->getContactPointA().Y, resultEpa->getContactPointB().Y);
    AssertHelper::instance()->assertFloatEquals(resultEpa->getContactPointA().Z, resultEpa->getContactPointB().Z);
}

void EPABoxTest::identicalOBBox() {
    CollisionBoxObject obbox1(0.0f, Vector3<float>(1.0f, 1.0f, 1.0f), Point3<float>(0.0f, 0.0f, 0.0f),
            Quaternion<float>(Vector3<float>(0.0f, 0.0f, 1.0f), 2.35619449f));
    CollisionBoxObject obbox2(0.0f, Vector3<float>(1.0f, 1.0f, 1.0f), Point3<float>(0.0f, 0.0f, 0.0f),
            Quaternion<float>(Vector3<float>(0.0f, 0.0f, 1.0f), 2.35619449f));

    std::shared_ptr<EPAResult<float>> resultEpa = EPATestHelper::executeEPA(obbox1, obbox2);

    AssertHelper::instance()->assertTrue(resultEpa->isCollide());
    AssertHelper::instance()->assertFloatEquals(resultEpa->getPenetrationDepth(), 2.0f);
    AssertHelper::instance()->assertFloatEquals(resultEpa->getContactPointA().vector(resultEpa->getContactPointB()).length(), 2.0f);
}

void EPABoxTest::overlapOnEdgeOBBox() {
    CollisionBoxObject obbox1(0.0f, Vector3<float>(1.0f, 1.0f, 1.0f), Point3<float>(0.0f, 0.0f, 0.0f),
            Quaternion<float>(Vector3<float>(0.0f, 0.0f, 1.0f), 2.35619449f));
    CollisionBoxObject obbox2(0.0f, Vector3<float>(1.0f, 1.0f, 1.0f), Point3<float>(2.5f, 0.0f, 0.0f),
            Quaternion<float>(Vector3<float>(0.0f, 0.0f, 1.0f), 2.35619449f));

    std::shared_ptr<EPAResult<float>> resultEpa = EPATestHelper::executeEPA(obbox1, obbox2);

    AssertHelper::instance()->assertTrue(resultEpa->isCollide());
    AssertHelper::instance()->assertFloatEquals(resultEpa->getPenetrationDepth(), 0.23223304702f);
    AssertHelper::instance()->assertFloatEquals(resultEpa->getNormal().X, 0.70710678118f);
    AssertHelper::instance()->assertFloatEquals(resultEpa->getContactPointA().vector(resultEpa->getContactPointB()).length(), 0.23223304702f);
    AssertHelper::instance()->assertFloatEquals(resultEpa->getContactPointA().Z, resultEpa->getContactPointB().Z);
}

void EPABoxTest::overlapOnCornerOBBox() {
    CollisionBoxObject obbox1(0.0f, Vector3<float>(1.0f, 1.0f, 1.0f), Point3<float>(0.0f, 0.0f, 0.0f),
            Quaternion<float>(Vector3<float>(0.245f, 0.769f, -0.59f), 0.987859f));
    CollisionBoxObject obbox2(0.0f, Vector3<float>(1.0f, 1.0f, 1.0f), Point3<float>(2.5f, 0.0f, 0.0f), Quaternion<float>());

    std::shared_ptr<EPAResult<float>> resultEpa = EPATestHelper::executeEPA(obbox1, obbox2);

    AssertHelper::instance()->assertTrue(resultEpa->isCollide());
    AssertHelper::instance()->assertFloatEquals(resultEpa->getPenetrationDepth(), 0.23205080757f);
    AssertHelper::instance()->assertVector3FloatEquals(resultEpa->getNormal(), Vector3<float>(1.0f, 0.0f, 0.0f));
    AssertHelper::instance()->assertFloatEquals(resultEpa->getContactPointA().vector(resultEpa->getContactPointB()).length(), 0.23205080757f);
    AssertHelper::instance()->assertFloatEquals(resultEpa->getContactPointA().X, 1.73205080757f);
    AssertHelper::instance()->assertFloatEquals(resultEpa->getContactPointB().X, 1.5f);
    AssertHelper::instance()->assertFloatEquals(resultEpa->getContactPointA().Y, resultEpa->getContactPointB().Y);
    AssertHelper::instance()->assertFloatEquals(resultEpa->getContactPointA().Z, resultEpa->getContactPointB().Z);
}

void EPABoxTest::overlapOBBoxAABBox() {
    CollisionBoxObject obbox(0.0f, Vector3<float>(1.0f, 1.0f, 1.0f), Point3<float>(0.0f, 0.0f, 0.0f),
            Quaternion<float>(Vector3<float>(0.0f, 0.0f, 1.0f), 2.35619449f));
    CollisionBoxObject aabbox(0.0f, Vector3<float>(1.0f, 1.0f, 1.0f), Point3<float>(2.0f, 0.0f, 0.0f), Quaternion<float>());

    std::shared_ptr<EPAResult<float>> resultEpa = EPATestHelper::executeEPA(obbox, aabbox);

    AssertHelper::instance()->assertTrue(resultEpa->isCollide());
    AssertHelper::instance()->assertFloatEquals(resultEpa->getPenetrationDepth(), 0.41421356237f);
    AssertHelper::instance()->assertVector3FloatEquals(resultEpa->getNormal(), Vector3<float>(1.0f, 0.0f, 0.0f));
    AssertHelper::instance()->assertFloatEquals(resultEpa->getContactPointA().vector(resultEpa->getContactPointB()).length(), 0.41421356237f);
    AssertHelper::instance()->assertFloatEquals(resultEpa->getContactPointA().X, 1.41421356237f);
    AssertHelper::instance()->assertFloatEquals(resultEpa->getContactPointA().Y, 0.0f);
    AssertHelper::instance()->assertFloatEquals(resultEpa->getContactPointB().X, 1.0f);
    AssertHelper::instance()->assertFloatEquals(resultEpa->getContactPointB().Y, 0.0f);
    AssertHelper::instance()->assertFloatEquals(resultEpa->getContactPointA().Z, resultEpa->getContactPointB().Z);
}

CppUnit::Test* EPABoxTest::suite() {
    auto* suite = new CppUnit::TestSuite("EPABoxTest");

    suite->addTest(new CppUnit::TestCaller<EPABoxTest>("identicalAABBox", &EPABoxTest::identicalAABBox));
    suite->addTest(new CppUnit::TestCaller<EPABoxTest>("overlapAABBox", &EPABoxTest::overlapAABBox));

    suite->addTest(new CppUnit::TestCaller<EPABoxTest>("identicalOBBox", &EPABoxTest::identicalOBBox));
    suite->addTest(new CppUnit::TestCaller<EPABoxTest>("overlapOnEdgeOBBox", &EPABoxTest::overlapOnEdgeOBBox));
    suite->addTest(new CppUnit::TestCaller<EPABoxTest>("overlapOnCornerOBBox", &EPABoxTest::overlapOnCornerOBBox));

    suite->addTest(new CppUnit::TestCaller<EPABoxTest>("overlapOBBoxAABBox", &EPABoxTest::overlapOBBoxAABBox));

    return suite;
}
