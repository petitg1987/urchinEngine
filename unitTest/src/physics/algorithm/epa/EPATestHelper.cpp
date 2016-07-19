#include "physics/algorithm/epa/EPATestHelper.h"
#include "physics/algorithm/gjk/GJKTestHelper.h"
using namespace urchin;

EPATestHelper::EPATestHelper()
{

}

EPATestHelper::~EPATestHelper()
{

}

std::shared_ptr<EPAResult<float>> EPATestHelper::executeEPA(const CollisionConvexObject3D &object1, const CollisionConvexObject3D &object2)
{
	return executeEPA(object1, object2, nullptr);
}

std::shared_ptr<EPAResult<float>> EPATestHelper::executeEPA(const CollisionConvexObject3D &object1, const CollisionConvexObject3D &object2, DebugRecorder<float> *debugRecorder)
{
	std::shared_ptr<GJKResult<float>> resultGjk = GJKTestHelper::executeGJK(object1, object2);

	EPAAlgorithm<float> epa;
	epa.setupDebugRecorder(debugRecorder);
	return epa.processEPA(object1, object2, *resultGjk.get());
}

