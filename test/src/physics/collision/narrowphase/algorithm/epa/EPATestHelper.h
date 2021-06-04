#pragma once

#include <memory>
#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>
using namespace urchin;

class EPATestHelper {
    public:
        static std::shared_ptr<EPAResult<float>> executeEPA(const CollisionConvexObject3D&, const CollisionConvexObject3D&);

    private:
        EPATestHelper() = default;
        ~EPATestHelper() = default;
};
