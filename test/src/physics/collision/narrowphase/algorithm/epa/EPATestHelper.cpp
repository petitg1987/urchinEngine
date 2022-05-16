#include <physics/collision/narrowphase/algorithm/epa/EPATestHelper.h>
#include <physics/collision/narrowphase/algorithm/gjk/GJKTestHelper.h>
using namespace urchin;

EPAResult<float> EPATestHelper::executeEPA(const CollisionConvexObject3D& object1, const CollisionConvexObject3D& object2) {
    GJKResult<float> resultGjk = GJKTestHelper::executeGJK(object1, object2);
    return EPAAlgorithm<float>().processEPA(object1, object2, resultGjk);
}
