#include <physics/collision/narrowphase/algorithm/gjk/GJKTestHelper.h>
using namespace urchin;

GJKResult<float> GJKTestHelper::executeGJK(const CollisionConvexObject3D& object1, const CollisionConvexObject3D& object2) {
    return GJKAlgorithm<float>().processGJK(object1, object2, true);
}
