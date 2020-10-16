#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <memory>
#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"

#include "AssertHelper.h"
#include "physics/collision/narrowphase/algorithm/epa/EPASphereTest.h"
#include "physics/collision/narrowphase/algorithm/epa/EPATestHelper.h"
using namespace urchin;

void EPASphereTest::identicalSphere() {
    CollisionSphereObject sphere1(1.0, Point3<float>(1.0, 0.0, 0.0));
    CollisionSphereObject sphere2(1.0, Point3<float>(1.0, 0.0, 0.0));

    std::shared_ptr<EPAResult<float>> resultEpa = EPATestHelper::executeEPA(sphere1, sphere2);

    float epsilon = 0.3f; //high epsilon used because curved shapes are very bad case for EPA
    AssertHelper::assertTrue(resultEpa->isCollide());
    AssertHelper::assertFloatEquals(resultEpa->getPenetrationDepth(), 2.0, epsilon);
    AssertHelper::assertFloatEquals(resultEpa->getContactPointA().vector(resultEpa->getContactPointB()).length(), 2.0, epsilon);
}

void EPASphereTest::overlapSphere() {
    CollisionSphereObject sphere1(10.0, Point3<float>(0.0, 0.0, 0.0));
    CollisionSphereObject sphere2(10.0, Point3<float>(7.0710678118+7.0, 7.0710678118+7.0, 0.0));

    std::shared_ptr<EPAResult<float>> resultEpa = EPATestHelper::executeEPA(sphere1, sphere2);

    float epsilon = 0.01f; //high epsilon used because curved shapes are very bad case for EPA
    AssertHelper::assertTrue(resultEpa->isCollide());
    AssertHelper::assertFloatEquals(resultEpa->getPenetrationDepth(), 0.10050506338);
    AssertHelper::assertVector3FloatEquals(resultEpa->getNormal(), Vector3<float>(0.70710678118, 0.70710678118, 0.0), epsilon);
    AssertHelper::assertFloatEquals(resultEpa->getContactPointA().X, 7.0710678118);
    AssertHelper::assertFloatEquals(resultEpa->getContactPointA().Y, 7.0710678118);
    AssertHelper::assertFloatEquals(resultEpa->getContactPointA().Z, 0.0);
    AssertHelper::assertFloatEquals(resultEpa->getContactPointB().X, 7.0);
    AssertHelper::assertFloatEquals(resultEpa->getContactPointB().Y, 7.0);
    AssertHelper::assertFloatEquals(resultEpa->getContactPointB().Z, 0.0);
}

CppUnit::Test *EPASphereTest::suite() {
    auto *suite = new CppUnit::TestSuite("EPASphereTest");

    suite->addTest(new CppUnit::TestCaller<EPASphereTest>("identicalSphere", &EPASphereTest::identicalSphere));
    suite->addTest(new CppUnit::TestCaller<EPASphereTest>("overlapSphere", &EPASphereTest::overlapSphere));

    return suite;
}
