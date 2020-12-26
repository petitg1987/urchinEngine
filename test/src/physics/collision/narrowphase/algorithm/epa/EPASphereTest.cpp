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
    CollisionSphereObject sphere1(1.0f, Point3<float>(1.0f, 0.0f, 0.0f));
    CollisionSphereObject sphere2(1.0f, Point3<float>(1.0f, 0.0f, 0.0f));

    std::shared_ptr<EPAResult<float>> resultEpa = EPATestHelper::executeEPA(sphere1, sphere2);

    float epsilon = 0.3f; //high epsilon used because curved shapes are very bad case for EPA
    AssertHelper::instance()->assertTrue(resultEpa->isCollide());
    AssertHelper::instance()->assertFloatEquals(resultEpa->getPenetrationDepth(), 2.0f, epsilon);
    AssertHelper::instance()->assertFloatEquals(resultEpa->getContactPointA().vector(resultEpa->getContactPointB()).length(), 2.0f, epsilon);
}

void EPASphereTest::overlapSphere() {
    CollisionSphereObject sphere1(10.0f, Point3<float>(0.0f, 0.0f, 0.0f));
    CollisionSphereObject sphere2(10.0f, Point3<float>(7.0710678118f + 7.0f, 7.0710678118f + 7.0f, 0.0f));

    std::shared_ptr<EPAResult<float>> resultEpa = EPATestHelper::executeEPA(sphere1, sphere2);

    float epsilon = 0.01f; //high epsilon used because curved shapes are very bad case for EPA
    AssertHelper::instance()->assertTrue(resultEpa->isCollide());
    AssertHelper::instance()->assertFloatEquals(resultEpa->getPenetrationDepth(), 0.10050506338f);
    AssertHelper::instance()->assertVector3FloatEquals(resultEpa->getNormal(), Vector3<float>(0.70710678118f, 0.70710678118f, 0.0f), epsilon);
    AssertHelper::instance()->assertFloatEquals(resultEpa->getContactPointA().X, 7.0710678118f);
    AssertHelper::instance()->assertFloatEquals(resultEpa->getContactPointA().Y, 7.0710678118f);
    AssertHelper::instance()->assertFloatEquals(resultEpa->getContactPointA().Z, 0.0f);
    AssertHelper::instance()->assertFloatEquals(resultEpa->getContactPointB().X, 7.0f);
    AssertHelper::instance()->assertFloatEquals(resultEpa->getContactPointB().Y, 7.0f);
    AssertHelper::instance()->assertFloatEquals(resultEpa->getContactPointB().Z, 0.0f);
}

CppUnit::Test* EPASphereTest::suite() {
    auto* suite = new CppUnit::TestSuite("EPASphereTest");

    suite->addTest(new CppUnit::TestCaller<EPASphereTest>("identicalSphere", &EPASphereTest::identicalSphere));
    suite->addTest(new CppUnit::TestCaller<EPASphereTest>("overlapSphere", &EPASphereTest::overlapSphere));

    return suite;
}
