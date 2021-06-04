#pragma once

#include <memory>
#include <UrchinPhysicsEngine.h>
#include <UrchinAIEngine.h>

namespace urchin {

    class AIEntityBuilder {
        public:
            AIEntityBuilder() = delete;

            static std::shared_ptr<AIObject> buildAIObject(const std::string&, const std::shared_ptr<const CollisionShape3D>&, const Transform<float>&);
            static std::shared_ptr<AITerrain> buildAITerrain(const std::string&, const std::shared_ptr<const CollisionShape3D>&, const Transform<float>&);

        private:
            static std::shared_ptr<AITerrain> buildAITerrain(const std::string&, const std::shared_ptr<const CollisionHeightfieldShape>&, const Transform<float>&);
    };

}
