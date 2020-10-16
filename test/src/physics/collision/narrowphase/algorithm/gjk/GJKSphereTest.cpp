#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <memory>
#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"

#include "AssertHelper.h"
#include "physics/collision/narrowphase/algorithm/gjk/GJKSphereTest.h"
#include "physics/collision/narrowphase/algorithm/gjk/GJKTestHelper.h"
using namespace urchin;

void GJKSphereTest::identicalSphere() {
    CollisionSphereObject sphere1(10.0, Point3<float>(1.0, 0.0, 0.0));
    CollisionSphereObject sphere2(10.0, Point3<float>(1.0, 0.0, 0.0));

    std::shared_ptr<GJKResult<float>> result = GJKTestHelper::executeGJK(sphere1, sphere2);

    AssertHelper::assertTrue(result->isCollide());
}

void GJKSphereTest::separateSphere() {
    CollisionSphereObject sphere1(10.0, Point3<float>(0.0, 0.0, 0.0));
    CollisionSphereObject sphere2(10.0, Point3<float>(7.0710678118+8.0, 7.0710678118+8.0, 0.0));

    std::shared_ptr<GJKResult<float>> result = GJKTestHelper::executeGJK(sphere1, sphere2);

    float epsilon = 0.01f; //high epsilon used because curved shapes are bad case for GJK
    AssertHelper::assertTrue(!result->isCollide());
    AssertHelper::assertFloatEquals(result->getSeparatingDistance(), 1.31370849889, epsilon);
    AssertHelper::assertPoint3FloatEquals(result->getClosestPointA(), Point3<float>(7.0710678118, 7.0710678118, 0.0), epsilon);
    AssertHelper::assertPoint3FloatEquals(result->getClosestPointB(), Point3<float>(8.0, 8.0, 0.0), epsilon);
}

void GJKSphereTest::overlapSphere() {
    CollisionSphereObject sphere1(10.0, Point3<float>(0.0, 0.0, 0.0));
    CollisionSphereObject sphere2(10.0, Point3<float>(7.0710678118+7.0, 7.0710678118+7.0, 0.0));

    std::shared_ptr<GJKResult<float>> result = GJKTestHelper::executeGJK(sphere1, sphere2);

    AssertHelper::assertTrue(result->isCollide());
}

CppUnit::Test *GJKSphereTest::suite() {
    auto *suite = new CppUnit::TestSuite("GJKSphereTest");

    suite->addTest(new CppUnit::TestCaller<GJKSphereTest>("identicalSphere", &GJKSphereTest::identicalSphere));
    suite->addTest(new CppUnit::TestCaller<GJKSphereTest>("separateSphere", &GJKSphereTest::separateSphere));
    suite->addTest(new CppUnit::TestCaller<GJKSphereTest>("overlapSphere", &GJKSphereTest::overlapSphere));

    return suite;
}
