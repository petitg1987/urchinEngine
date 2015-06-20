#include "physics/algorithm/gjk/GJKTestHelper.h"
using namespace urchin;

GJKTestHelper::GJKTestHelper()
{

}

GJKTestHelper::~GJKTestHelper()
{

}

std::shared_ptr<GJKResult<float>> GJKTestHelper::executeGJK(const CollisionConvexObject3D &object1, const CollisionConvexObject3D &object2)
{
	GJKAlgorithm<float> gjk;
	return gjk.processGJK(object1, object2, true);
}
