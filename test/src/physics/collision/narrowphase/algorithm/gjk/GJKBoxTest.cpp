#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <memory>
#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"

#include "AssertHelper.h"
#include "physics/collision/narrowphase/algorithm/gjk/GJKBoxTest.h"
#include "physics/collision/narrowphase/algorithm/gjk/GJKTestHelper.h"
using namespace urchin;

void GJKBoxTest::identicalAABBox() {
    CollisionBoxObject aabbox1(0.0, Vector3<float>(0.5, 0.5, 0.5), Point3<float>(0.5, 0.5, 0.5), Quaternion<float>());
    CollisionBoxObject aabbox2(0.0, Vector3<float>(0.5, 0.5, 0.5), Point3<float>(0.5, 0.5, 0.5), Quaternion<float>());

    std::shared_ptr<GJKResult<float>> result = GJKTestHelper::executeGJK(aabbox1, aabbox2);

    AssertHelper::assertTrue(result->isCollide());
}

void GJKBoxTest::separateAABBox() {
    CollisionBoxObject aabbox1(0.0, Vector3<float>(0.5, 0.5, 0.5), Point3<float>(0.5, 0.5, 0.5), Quaternion<float>());
    CollisionBoxObject aabbox2(0.0, Vector3<float>(0.5, 0.5, 0.5), Point3<float>(-0.6, 0.5, 0.5), Quaternion<float>());

    std::shared_ptr<GJKResult<float>> result = GJKTestHelper::executeGJK(aabbox1, aabbox2);

    AssertHelper::assertTrue(!result->isCollide());
    AssertHelper::assertFloatEquals(result->getSeparatingDistance(), 0.1);
    AssertHelper::assertFloatEquals(result->getClosestPointA().X, 0.0);
    AssertHelper::assertFloatEquals(result->getClosestPointB().X, -0.1);
    AssertHelper::assertFloatEquals(result->getClosestPointB().vector(result->getClosestPointA()).length(), 0.1);
}

void GJKBoxTest::overlapAABBox() {
    CollisionBoxObject aabbox1(0.0, Vector3<float>(0.5, 0.5, 0.5), Point3<float>(0.5, 0.5, 0.5), Quaternion<float>());
    CollisionBoxObject aabbox2(0.0, Vector3<float>(0.5, 0.5, 0.5), Point3<float>(0.0, 0.5, 0.5), Quaternion<float>());

    std::shared_ptr<GJKResult<float>> result = GJKTestHelper::executeGJK(aabbox1, aabbox2);

    AssertHelper::assertTrue(result->isCollide());
}

void GJKBoxTest::identicalOBBox() {
    CollisionBoxObject obbox1(0.0, Vector3<float>(1.0, 1.0, 1.0), Point3<float>(0.0, 0.0, 0.0),
            Quaternion<float>(Vector3<float>(0.0, 0.0, 1.0), 2.35619449));
    CollisionBoxObject obbox2(0.0, Vector3<float>(1.0, 1.0, 1.0), Point3<float>(0.0, 0.0, 0.0),
            Quaternion<float>(Vector3<float>(0.0, 0.0, 1.0), 2.35619449));

    std::shared_ptr<GJKResult<float>> result = GJKTestHelper::executeGJK(obbox1, obbox2);

    AssertHelper::assertTrue(result->isCollide());
}

void GJKBoxTest::separateEdgeOBBox() {
    CollisionBoxObject obbox1(0.0, Vector3<float>(1.0, 1.0, 1.0), Point3<float>(0.0, 0.0, 0.0),
            Quaternion<float>(Vector3<float>(0.0, 0.0, 1.0), 2.35619449));
    CollisionBoxObject obbox2(0.0, Vector3<float>(1.0, 1.0, 1.0), Point3<float>(3.0, 0.0, 0.0),
            Quaternion<float>(Vector3<float>(0.0, 0.0, 1.0), 2.35619449));

    std::shared_ptr<GJKResult<float>> result = GJKTestHelper::executeGJK(obbox1, obbox2);

    AssertHelper::assertTrue(!result->isCollide());
    AssertHelper::assertFloatEquals(result->getClosestPointA().Y, 0.0);
    AssertHelper::assertFloatEquals(result->getClosestPointB().X, 1.58578643763);
    AssertHelper::assertFloatEquals(result->getClosestPointB().Y, 0.0);
    AssertHelper::assertFloatEquals(result->getClosestPointB().vector(result->getClosestPointA()).length(), 0.17157287526);
}

void GJKBoxTest::separateCornerOBBox() {
    CollisionBoxObject obbox1(0.0, Vector3<float>(1.0, 1.0, 1.0), Point3<float>(0.0, 0.0, 0.0),
            Quaternion<float>(Vector3<float>(0.245, 0.769, -0.59), 0.987859));
    CollisionBoxObject obbox2(0.0, Vector3<float>(1.0, 1.0, 1.0), Point3<float>(3.0, 0.0, 0.0), Quaternion<float>());

    std::shared_ptr<GJKResult<float>> result = GJKTestHelper::executeGJK(obbox1, obbox2);

    AssertHelper::assertTrue(!result->isCollide());
    AssertHelper::assertFloatEquals(result->getSeparatingDistance(), 0.26794919243);
    AssertHelper::assertPoint3FloatEquals(result->getClosestPointA(), Point3<float>(1.73205080757, 0.0, 0.0));
    AssertHelper::assertPoint3FloatEquals(result->getClosestPointB(), Point3<float>(2.0, 0.0, 0.0));
}

void GJKBoxTest::overlapOnEdgeOBBox() {
    CollisionBoxObject obbox1(0.0, Vector3<float>(1.0, 1.0, 1.0), Point3<float>(0.0, 0.0, 0.0),
            Quaternion<float>(Vector3<float>(0.0, 0.0, 1.0), 2.35619449));
    CollisionBoxObject obbox2(0.0, Vector3<float>(1.0, 1.0, 1.0), Point3<float>(2.0, 0.0, 0.0),
            Quaternion<float>(Vector3<float>(0.0, 0.0, 1.0), 2.35619449));

    std::shared_ptr<GJKResult<float>> result = GJKTestHelper::executeGJK(obbox1, obbox2);

    AssertHelper::assertTrue(result->isCollide());
}

void GJKBoxTest::separateOBBoxAABBox() {
    CollisionBoxObject obbox(0.0, Vector3<float>(1.0, 1.0, 1.0), Point3<float>(0.0, 0.0, 0.0),
            Quaternion<float>(Vector3<float>(0.0, 0.0, 1.0), 2.35619449));
    CollisionBoxObject aabbox(0.0, Vector3<float>(1.0, 1.0, 1.0), Point3<float>(3.0, 0.0, 0.0), Quaternion<float>());

    std::shared_ptr<GJKResult<float>> result = GJKTestHelper::executeGJK(obbox, aabbox);

    AssertHelper::assertTrue(!result->isCollide());
    AssertHelper::assertFloatEquals(result->getSeparatingDistance(), 0.58578643763);
    AssertHelper::assertFloatEquals(result->getClosestPointA().X, 1.41421356237);
    AssertHelper::assertFloatEquals(result->getClosestPointA().Y, 0.0);
    AssertHelper::assertFloatEquals(result->getClosestPointB().X, 2.0);
    AssertHelper::assertFloatEquals(result->getClosestPointB().Y, 0.0);
    AssertHelper::assertFloatEquals(result->getClosestPointB().vector(result->getClosestPointA()).length(), 0.58578643763);
}

void GJKBoxTest::overlapOBBoxAABBox() {
    CollisionBoxObject obbox(0.0, Vector3<float>(1.0, 1.0, 1.0), Point3<float>(0.0, 0.0, 0.0),
            Quaternion<float>(Vector3<float>(0.0, 0.0, 1.0), 2.35619449));
    CollisionBoxObject aabbox(0.0, Vector3<float>(1.0, 1.0, 1.0), Point3<float>(2.0, 0.0, 0.0), Quaternion<float>());

    std::shared_ptr<GJKResult<float>> result = GJKTestHelper::executeGJK(obbox, aabbox);

    AssertHelper::assertTrue(result->isCollide());
}

CppUnit::Test *GJKBoxTest::suite() {
    auto* suite = new CppUnit::TestSuite("GJKBoxTest");

    suite->addTest(new CppUnit::TestCaller<GJKBoxTest>("identicalAABBox", &GJKBoxTest::identicalAABBox));
    suite->addTest(new CppUnit::TestCaller<GJKBoxTest>("separateAABBox", &GJKBoxTest::separateAABBox));
    suite->addTest(new CppUnit::TestCaller<GJKBoxTest>("overlapAABBox", &GJKBoxTest::overlapAABBox));

    suite->addTest(new CppUnit::TestCaller<GJKBoxTest>("identicalOBBox", &GJKBoxTest::identicalOBBox));
    suite->addTest(new CppUnit::TestCaller<GJKBoxTest>("separateEdgeOBBox", &GJKBoxTest::separateEdgeOBBox));
    suite->addTest(new CppUnit::TestCaller<GJKBoxTest>("separateCornerOBBox", &GJKBoxTest::separateCornerOBBox));
    suite->addTest(new CppUnit::TestCaller<GJKBoxTest>("overlapOnEdgeOBBox", &GJKBoxTest::overlapOnEdgeOBBox));

    suite->addTest(new CppUnit::TestCaller<GJKBoxTest>("separateOBBoxAABBox", &GJKBoxTest::separateOBBoxAABBox));
    suite->addTest(new CppUnit::TestCaller<GJKBoxTest>("overlapOBBoxAABBox", &GJKBoxTest::overlapOBBoxAABBox));

    return suite;
}
