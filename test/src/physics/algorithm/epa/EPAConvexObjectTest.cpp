#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <memory>
#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"

#include "AssertHelper.h"
#include "physics/algorithm/epa/EPAConvexObjectTest.h"
#include "physics/algorithm/epa/EPATestHelper.h"
using namespace urchin;

void EPAConvexObjectTest::overlapSphereAndBox()
{
	CollisionSphereObject sphere(1.0, Point3<float>(0.0, 0.0, 0.0));
	CollisionBoxObject aabbox(0.0, Vector3<float>(0.5, 0.5, 0.5), Point3<float>(1.4, 0.5, 0.5), Quaternion<float>());

	std::shared_ptr<EPAResult<float>> resultEpa = EPATestHelper::executeEPA(sphere, aabbox);

	float epsilon = 0.05f; //high epsilon used because curved shapes are very bad case for EPA
	AssertHelper::assertTrue(resultEpa->isCollide());
	AssertHelper::assertFloatEquals(resultEpa->getPenetrationDepth(), 0.1);
	AssertHelper::assertVector3FloatEquals(resultEpa->getNormal(), Vector3<float>(1.0, 0.0, 0.0), epsilon);
	AssertHelper::assertFloatEquals(resultEpa->getContactPointA().vector(resultEpa->getContactPointB()).length(), 0.1, epsilon);
    AssertHelper::assertPoint3FloatEquals(resultEpa->getContactPointA(), Point3<float>(1.0, 0.0, 0.0), epsilon);
    AssertHelper::assertPoint3FloatEquals(resultEpa->getContactPointB(), Point3<float>(0.9, 0.0, 0.0), epsilon);
}

void EPAConvexObjectTest::overlapCapsuleAndTriangle()
{
	CollisionCapsuleObject capsule(0.0f, 0.25f, 1.0f, CapsuleShape<float>::CAPSULE_Y, Point3<float>(0.0, 0.2, 0.0), Quaternion<float>());
	CollisionTriangleObject triangle(0.0f, Point3<float>(0.0, 0.0, 0.0), Point3<float>(-2.0, 0.0, -2.0), Point3<float>(-2.0, 0.0, 0.0));

	std::shared_ptr<EPAResult<float>> resultEpa = EPATestHelper::executeEPA(capsule, triangle);

	AssertHelper::assertTrue(resultEpa->isCollide());
	AssertHelper::assertFloatEquals(resultEpa->getPenetrationDepth(), 0.55);
    AssertHelper::assertVector3FloatEquals(resultEpa->getNormal(), Vector3<float>(0.0, -1.0, 0.0));
    AssertHelper::assertPoint3FloatEquals(resultEpa->getContactPointA(), Point3<float>(0.0, -0.55, 0.0));
    AssertHelper::assertPoint3FloatEquals(resultEpa->getContactPointB(), Point3<float>(0.0, 0.0, 0.0));
}

void EPAConvexObjectTest::overlapTriangleAndCapsule()
{
	CollisionTriangleObject triangle(0.0f, Point3<float>(0.0, 0.0, 0.0), Point3<float>(-2.0, 0.0, -2.0), Point3<float>(-2.0, 0.0, 0.0));
	CollisionCapsuleObject capsule(0.0f, 0.25f, 1.0f, CapsuleShape<float>::CAPSULE_Y, Point3<float>(0.0, 0.2, 0.0), Quaternion<float>());

	std::shared_ptr<EPAResult<float>> resultEpa = EPATestHelper::executeEPA(triangle, capsule);

	AssertHelper::assertTrue(resultEpa->isCollide());
	AssertHelper::assertFloatEquals(resultEpa->getPenetrationDepth(), 0.55);
	AssertHelper::assertVector3FloatEquals(resultEpa->getNormal(), Vector3<float>(0.0, 1.0, 0.0));
    AssertHelper::assertPoint3FloatEquals(resultEpa->getContactPointA(), Point3<float>(0.0, 0.0, 0.0));
    AssertHelper::assertPoint3FloatEquals(resultEpa->getContactPointB(), Point3<float>(0.0, -0.55, 0.0));
}

CppUnit::Test *EPAConvexObjectTest::suite()
{
    auto *suite = new CppUnit::TestSuite("EPAConvexObjectTest");

	suite->addTest(new CppUnit::TestCaller<EPAConvexObjectTest>("overlapSphereAndBox", &EPAConvexObjectTest::overlapSphereAndBox));
	suite->addTest(new CppUnit::TestCaller<EPAConvexObjectTest>("overlapCapsuleAndTriangle", &EPAConvexObjectTest::overlapCapsuleAndTriangle));
	suite->addTest(new CppUnit::TestCaller<EPAConvexObjectTest>("overlapTriangleAndCapsule", &EPAConvexObjectTest::overlapTriangleAndCapsule));

	return suite;
}
