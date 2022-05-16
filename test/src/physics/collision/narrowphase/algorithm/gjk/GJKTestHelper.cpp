#include <physics/collision/narrowphase/algorithm/gjk/GJKTestHelper.h>
using namespace urchin;

GJKResult<float> GJKTestHelper::executeGJK(const CollisionConvexObject3D& object1, const CollisionConvexObject3D& object2) {
    return GJKAlgorithm<float>().processGJK(GJKConvexObjectWrapper(object1, true), GJKConvexObjectWrapper(object2, true));
}
