#pragma once

#include <memory>
#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>

class EPATestHelper {
    public:
        static std::shared_ptr<urchin::EPAResult<float>> executeEPA(const urchin::CollisionConvexObject3D&, const urchin::CollisionConvexObject3D&);

    private:
        EPATestHelper() = default;
        ~EPATestHelper() = default;
};
