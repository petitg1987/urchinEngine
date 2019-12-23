#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <memory>
#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"

#include "AssertHelper.h"
#include "physics/collision/narrowphase/algorithm/epa/EPABoxTest.h"
#include "physics/collision/narrowphase/algorithm/epa/EPATestHelper.h"
using namespace urchin;

void EPABoxTest::identicalAABBox()
{
	CollisionBoxObject aabbox1(0.0, Vector3<float>(0.5, 0.5, 0.5), Point3<float>(0.5, 0.5, 0.5), Quaternion<float>());
	CollisionBoxObject aabbox2(0.0, Vector3<float>(0.5, 0.5, 0.5), Point3<float>(0.5, 0.5, 0.5), Quaternion<float>());

	std::shared_ptr<EPAResult<float>> resultEpa = EPATestHelper::executeEPA(aabbox1, aabbox2);

	AssertHelper::assertTrue(resultEpa->isCollide());
	AssertHelper::assertFloatEquals(resultEpa->getPenetrationDepth(), 1.0);
	AssertHelper::assertFloatEquals(resultEpa->getContactPointA().vector(resultEpa->getContactPointB()).length(), 1.0);
}

void EPABoxTest::overlapAABBox()
{
	CollisionBoxObject aabbox1(0.0, Vector3<float>(0.5, 0.5, 0.5), Point3<float>(0.5, 0.5, 0.5), Quaternion<float>());
	CollisionBoxObject aabbox2(0.0, Vector3<float>(0.5, 0.5, 0.5), Point3<float>(1.1, 0.5, 0.5), Quaternion<float>());

	std::shared_ptr<EPAResult<float>> resultEpa = EPATestHelper::executeEPA(aabbox1, aabbox2);

	AssertHelper::assertTrue(resultEpa->isCollide());
	AssertHelper::assertFloatEquals(resultEpa->getPenetrationDepth(), 0.4);
	AssertHelper::assertVector3FloatEquals(resultEpa->getNormal(), Vector3<float>(1.0, 0.0, 0.0));
	AssertHelper::assertFloatEquals(resultEpa->getContactPointA().vector(resultEpa->getContactPointB()).length(), 0.4);
	AssertHelper::assertFloatEquals(resultEpa->getContactPointA().X, 1.0);
	AssertHelper::assertFloatEquals(resultEpa->getContactPointB().X, 0.6);
	AssertHelper::assertFloatEquals(resultEpa->getContactPointA().Y, resultEpa->getContactPointB().Y);
	AssertHelper::assertFloatEquals(resultEpa->getContactPointA().Z, resultEpa->getContactPointB().Z);
}

void EPABoxTest::identicalOBBox()
{
	CollisionBoxObject obbox1(0.0, Vector3<float>(1.0, 1.0, 1.0), Point3<float>(0.0, 0.0, 0.0),
			Quaternion<float>(Vector3<float>(0.0, 0.0, 1.0), 2.35619449));
	CollisionBoxObject obbox2(0.0, Vector3<float>(1.0, 1.0, 1.0), Point3<float>(0.0, 0.0, 0.0),
			Quaternion<float>(Vector3<float>(0.0, 0.0, 1.0), 2.35619449));

	std::shared_ptr<EPAResult<float>> resultEpa = EPATestHelper::executeEPA(obbox1, obbox2);

	AssertHelper::assertTrue(resultEpa->isCollide());
	AssertHelper::assertFloatEquals(resultEpa->getPenetrationDepth(), 2.0);
	AssertHelper::assertFloatEquals(resultEpa->getContactPointA().vector(resultEpa->getContactPointB()).length(), 2.0);
}

void EPABoxTest::overlapOnEdgeOBBox()
{
	CollisionBoxObject obbox1(0.0, Vector3<float>(1.0, 1.0, 1.0), Point3<float>(0.0, 0.0, 0.0),
			Quaternion<float>(Vector3<float>(0.0, 0.0, 1.0), 2.35619449));
	CollisionBoxObject obbox2(0.0, Vector3<float>(1.0, 1.0, 1.0), Point3<float>(2.5, 0.0, 0.0),
			Quaternion<float>(Vector3<float>(0.0, 0.0, 1.0), 2.35619449));

	std::shared_ptr<EPAResult<float>> resultEpa = EPATestHelper::executeEPA(obbox1, obbox2);

	AssertHelper::assertTrue(resultEpa->isCollide());
	AssertHelper::assertFloatEquals(resultEpa->getPenetrationDepth(), 0.23223304702);
	AssertHelper::assertFloatEquals(resultEpa->getNormal().X, 0.70710678118);
	AssertHelper::assertFloatEquals(resultEpa->getContactPointA().vector(resultEpa->getContactPointB()).length(), 0.23223304702);
	AssertHelper::assertFloatEquals(resultEpa->getContactPointA().Z, resultEpa->getContactPointB().Z);
}

void EPABoxTest::overlapOnCornerOBBox()
{
	CollisionBoxObject obbox1(0.0, Vector3<float>(1.0, 1.0, 1.0), Point3<float>(0.0, 0.0, 0.0),
			Quaternion<float>(Vector3<float>(0.245, 0.769, -0.59), 0.987859));
	CollisionBoxObject obbox2(0.0, Vector3<float>(1.0, 1.0, 1.0), Point3<float>(2.5, 0.0, 0.0), Quaternion<float>());

	std::shared_ptr<EPAResult<float>> resultEpa = EPATestHelper::executeEPA(obbox1, obbox2);

	AssertHelper::assertTrue(resultEpa->isCollide());
	AssertHelper::assertFloatEquals(resultEpa->getPenetrationDepth(), 0.23205080757);
	AssertHelper::assertVector3FloatEquals(resultEpa->getNormal(), Vector3<float>(1.0, 0.0, 0.0));
	AssertHelper::assertFloatEquals(resultEpa->getContactPointA().vector(resultEpa->getContactPointB()).length(), 0.23205080757);
	AssertHelper::assertFloatEquals(resultEpa->getContactPointA().X, 1.73205080757);
	AssertHelper::assertFloatEquals(resultEpa->getContactPointB().X, 1.5);
	AssertHelper::assertFloatEquals(resultEpa->getContactPointA().Y, resultEpa->getContactPointB().Y);
	AssertHelper::assertFloatEquals(resultEpa->getContactPointA().Z, resultEpa->getContactPointB().Z);
}

void EPABoxTest::overlapOBBoxAABBox()
{
	CollisionBoxObject obbox(0.0, Vector3<float>(1.0, 1.0, 1.0), Point3<float>(0.0, 0.0, 0.0),
			Quaternion<float>(Vector3<float>(0.0, 0.0, 1.0), 2.35619449));
	CollisionBoxObject aabbox(0.0, Vector3<float>(1.0, 1.0, 1.0), Point3<float>(2.0, 0.0, 0.0), Quaternion<float>());

	std::shared_ptr<EPAResult<float>> resultEpa = EPATestHelper::executeEPA(obbox, aabbox);

	AssertHelper::assertTrue(resultEpa->isCollide());
	AssertHelper::assertFloatEquals(resultEpa->getPenetrationDepth(), 0.41421356237);
	AssertHelper::assertVector3FloatEquals(resultEpa->getNormal(), Vector3<float>(1.0, 0.0, 0.0));
	AssertHelper::assertFloatEquals(resultEpa->getContactPointA().vector(resultEpa->getContactPointB()).length(), 0.41421356237);
	AssertHelper::assertFloatEquals(resultEpa->getContactPointA().X, 1.41421356237);
	AssertHelper::assertFloatEquals(resultEpa->getContactPointA().Y, 0.0);
	AssertHelper::assertFloatEquals(resultEpa->getContactPointB().X, 1.0);
	AssertHelper::assertFloatEquals(resultEpa->getContactPointB().Y, 0.0);
	AssertHelper::assertFloatEquals(resultEpa->getContactPointA().Z, resultEpa->getContactPointB().Z);
}

CppUnit::Test *EPABoxTest::suite()
{
    auto *suite = new CppUnit::TestSuite("EPABoxTest");

	suite->addTest(new CppUnit::TestCaller<EPABoxTest>("identicalAABBox", &EPABoxTest::identicalAABBox));
	suite->addTest(new CppUnit::TestCaller<EPABoxTest>("overlapAABBox", &EPABoxTest::overlapAABBox));

	suite->addTest(new CppUnit::TestCaller<EPABoxTest>("identicalOBBox", &EPABoxTest::identicalOBBox));
	suite->addTest(new CppUnit::TestCaller<EPABoxTest>("overlapOnEdgeOBBox", &EPABoxTest::overlapOnEdgeOBBox));
	suite->addTest(new CppUnit::TestCaller<EPABoxTest>("overlapOnCornerOBBox", &EPABoxTest::overlapOnCornerOBBox));

	suite->addTest(new CppUnit::TestCaller<EPABoxTest>("overlapOBBoxAABBox", &EPABoxTest::overlapOBBoxAABBox));

	return suite;
}
