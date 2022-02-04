#pragma once

#include <memory>
#include <UrchinPhysicsEngine.h>
#include <UrchinAIEngine.h>

namespace urchin {

    class AIEntityBuilder {
        public:
            AIEntityBuilder() = delete;

            static std::shared_ptr<AIObject> buildAIObject(std::string, const CollisionShape3D&, const Transform<float>&);
            static std::shared_ptr<AITerrain> buildAITerrain(std::string, const CollisionShape3D&, const Transform<float>&);
    };

}
