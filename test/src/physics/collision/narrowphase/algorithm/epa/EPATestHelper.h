#pragma once

#include <memory>
#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>

class EPATestHelper {
    public:
        static urchin::EPAResult<float> executeEPA(const urchin::CollisionConvexObject3D&, const urchin::CollisionConvexObject3D&);

    private:
        EPATestHelper() = default;
        ~EPATestHelper() = default;
};
