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

void EPAConvexObjectTest::lightlyContactBoxAndCapsule()
{
	//capsule is laid on the tilted ground
	CollisionBoxObject box(9.29832e-06f, Vector3<float>(68.7868881f, 68.7868881f, 3.71932801e-05f), Point3<float>(-2.f, -2.75f, 0.f),
			Quaternion<float>(-0.642790139f, 0.f, 0.f, 0.766042292f));
	CollisionCapsuleObject capsule(0.0384728f, 0.153891206f, 0.657813013f, CapsuleShape<float>::CapsuleOrientation::CAPSULE_Z,
			Point3<float>(-2.72755051f, -6.43558121f, 22.0171185f), Quaternion<float>(0.134508997f, -0.0345658399f, -0.59449321f, 0.792016685f));

	std::shared_ptr<EPAResult<float>> resultEpa = EPATestHelper::executeEPA(box, capsule);

	AssertHelper::assertTrue(!resultEpa->isValidResult());
}

void EPAConvexObjectTest::lightlyContactBoxAndCapsule2()
{
	//capsule is laid on the tilted ground
	CollisionBoxObject box(0.0186000001f, Vector3<float>(130.720001f, 130.7202f, 0.0743999928f), Point3<float>(-2.0f, -2.75f, 0.0f),
			Quaternion<float>(-0.706433773f, -0.0308436025f, 0.0308436025f, 0.706433773f));
	CollisionCapsuleObject capsule(0.0399999991, 0.210000008f, 1.0f, CapsuleShape<float>::CapsuleOrientation::CAPSULE_Y,
			Point3<float>(-11.2144461f, -2.66024923f, -7.12916136f), Quaternion<float>(0.f, 0.f, 0.f, 1.f));

	std::shared_ptr<EPAResult<float>> resultEpa = EPATestHelper::executeEPA(capsule, box);

	//TODO: is value is correct ?
	std::cout<<resultEpa->getPenetrationDepth()<<std::endl;
}

CppUnit::Test *EPAConvexObjectTest::suite()
{
	CppUnit::TestSuite *suite = new CppUnit::TestSuite("EPAConvexObjectTest");

	suite->addTest(new CppUnit::TestCaller<EPAConvexObjectTest>("overlapSphereAndBox", &EPAConvexObjectTest::overlapSphereAndBox));
	suite->addTest(new CppUnit::TestCaller<EPAConvexObjectTest>("lightlyContactBoxAndCapsule", &EPAConvexObjectTest::lightlyContactBoxAndCapsule));
	suite->addTest(new CppUnit::TestCaller<EPAConvexObjectTest>("lightlyContactBoxAndCapsule2", &EPAConvexObjectTest::lightlyContactBoxAndCapsule2));

	return suite;
}
