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
	AssertHelper::assertFloatEquals(resultEpa->getContactPointA().X, 1.0, epsilon);
	AssertHelper::assertFloatEquals(resultEpa->getContactPointA().Y, 0.0, epsilon);
	AssertHelper::assertFloatEquals(resultEpa->getContactPointA().Z, 0.0, epsilon);
	AssertHelper::assertFloatEquals(resultEpa->getContactPointB().X, 0.9, epsilon);
	AssertHelper::assertFloatEquals(resultEpa->getContactPointB().Y, 0.0, epsilon);
	AssertHelper::assertFloatEquals(resultEpa->getContactPointB().Z, 0.0, epsilon);
}

CppUnit::Test *EPAConvexObjectTest::suite()
{
	CppUnit::TestSuite *suite = new CppUnit::TestSuite("EPAConvexObjectTest");

	suite->addTest(new CppUnit::TestCaller<EPAConvexObjectTest>("overlapSphereAndBox", &EPAConvexObjectTest::overlapSphereAndBox));

	return suite;
}
