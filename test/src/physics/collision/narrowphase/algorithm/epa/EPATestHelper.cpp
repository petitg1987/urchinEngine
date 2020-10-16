#include "physics/collision/narrowphase/algorithm/epa/EPATestHelper.h"
#include "physics/collision/narrowphase/algorithm/gjk/GJKTestHelper.h"
using namespace urchin;

std::shared_ptr<EPAResult<float>> EPATestHelper::executeEPA(const CollisionConvexObject3D &object1, const CollisionConvexObject3D &object2) {
    std::shared_ptr<GJKResult<float>> resultGjk = GJKTestHelper::executeGJK(object1, object2);

    EPAAlgorithm<float> epa;
    return epa.processEPA(object1, object2, *resultGjk.get());
}
