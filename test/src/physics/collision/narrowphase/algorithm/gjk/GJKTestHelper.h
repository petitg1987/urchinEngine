#pragma once

#include <memory>
#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>

class GJKTestHelper {
    public:
        static urchin::GJKResult<float> executeGJK(const urchin::CollisionConvexObject3D&, const urchin::CollisionConvexObject3D&);

    private:
        GJKTestHelper() = default;
        ~GJKTestHelper() = default;
};
