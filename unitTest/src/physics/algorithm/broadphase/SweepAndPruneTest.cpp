#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"

#include "AssertHelper.h"
#include "physics/algorithm/broadphase/SweepAndPruneTest.h"
#include "physics/algorithm/broadphase/BodyTestHelper.h"
using namespace urchin;

void SweepAndPruneTest::moveBodyCollide()
{
	std::unique_ptr<WorkRigidBody> body1 = BodyTestHelper::createCubeRigidBody(Point3<float>(0.0, 0.0, 0.0), 2.0);
	std::unique_ptr<WorkRigidBody> body2 = BodyTestHelper::createCubeRigidBody(Point3<float>(-3.0, 0.0, 0.0), 1.0);

	SweepAndPrune sweepAndPrune;
	sweepAndPrune.addBody(body1.get(), nullptr);
	sweepAndPrune.addBody(body2.get(), nullptr);
	body2->setPosition(Point3<float>(0.1, 0.0, 0.0));
	sweepAndPrune.updateBodies();

	const std::vector<OverlappingPair *> &overlappingPairs = sweepAndPrune.getOverlappingPairs();
	AssertHelper::assertUnsignedInt(overlappingPairs.size(), 1);
}

void SweepAndPruneTest::moveBodyNotCollide()
{
	std::unique_ptr<WorkRigidBody> body1 = BodyTestHelper::createCubeRigidBody(Point3<float>(0.0, 0.0, 0.0), 2.0);
	std::unique_ptr<WorkRigidBody> body2 = BodyTestHelper::createCubeRigidBody(Point3<float>(-3.0, 0.0, 0.0), 1.0);

	SweepAndPrune sweepAndPrune;
	sweepAndPrune.addBody(body1.get(), nullptr);
	sweepAndPrune.addBody(body2.get(), nullptr);
	body2->setPosition(Point3<float>(0.0, -3.0, 0.0));
	sweepAndPrune.updateBodies();

	const std::vector<OverlappingPair *> &overlappingPairs = sweepAndPrune.getOverlappingPairs();
	AssertHelper::assertUnsignedInt(overlappingPairs.size(), 0);
}

CppUnit::Test *SweepAndPruneTest::suite()
{
	CppUnit::TestSuite *suite = new CppUnit::TestSuite("SweepAndPruneTest");

	suite->addTest(new CppUnit::TestCaller<SweepAndPruneTest>("moveBodyCollide", &SweepAndPruneTest::moveBodyCollide));
	suite->addTest(new CppUnit::TestCaller<SweepAndPruneTest>("moveBodyNotCollide", &SweepAndPruneTest::moveBodyNotCollide));

	return suite;
}
