#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <memory>
#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"

#include "AssertHelper.h"
#include "physics/collision/narrowphase/algorithm/gjk/GJKConvexObjectTest.h"
#include "physics/collision/narrowphase/algorithm/gjk/GJKTestHelper.h"
using namespace urchin;

void GJKConvexObjectTest::separateSphereAndBox()
{
	CollisionSphereObject sphere(1.0, Point3<float>(0.0, 0.0, 0.0));
	CollisionBoxObject aabbox(0.0, Vector3<float>(0.5, 0.5, 0.5), Point3<float>(1.6, 0.5, 0.5), Quaternion<float>());

	std::shared_ptr<GJKResult<float>> result = GJKTestHelper::executeGJK(sphere, aabbox);

	AssertHelper::assertTrue(!result->isCollide());
	AssertHelper::assertFloatEquals(result->getSeparatingDistance(), 0.1);
}

void GJKConvexObjectTest::overlapSphereAndBox()
{
	CollisionSphereObject sphere(1.0, Point3<float>(0.0, 0.0, 0.0));
	CollisionBoxObject aabbox(0.0, Vector3<float>(0.5, 0.5, 0.5), Point3<float>(1.4, 0.5, 0.5), Quaternion<float>());

	std::shared_ptr<GJKResult<float>> result = GJKTestHelper::executeGJK(sphere, aabbox);

	AssertHelper::assertTrue(result->isCollide());
}

CppUnit::Test *GJKConvexObjectTest::suite()
{
    auto *suite = new CppUnit::TestSuite("GJKConvexObjectTest");

	suite->addTest(new CppUnit::TestCaller<GJKConvexObjectTest>("separateSphereAndBox", &GJKConvexObjectTest::separateSphereAndBox));
	suite->addTest(new CppUnit::TestCaller<GJKConvexObjectTest>("overlapSphereAndBox", &GJKConvexObjectTest::overlapSphereAndBox));

	return suite;
}
