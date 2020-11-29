#include "physics/collision/narrowphase/algorithm/gjk/GJKTestHelper.h"
using namespace urchin;

std::shared_ptr<GJKResult<float>> GJKTestHelper::executeGJK(const CollisionConvexObject3D& object1, const CollisionConvexObject3D& object2) {
    GJKAlgorithm<float> gjk;
    return gjk.processGJK(object1, object2, true);
}
