#ifndef URCHINENGINE_AIOBJECTBUILDER_H
#define URCHINENGINE_AIOBJECTBUILDER_H

#include <memory>
#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"
#include "UrchinAIEngine.h"

namespace urchin
{

    class AIEntityBuilder : public Singleton<AIEntityBuilder>
    {
        public:
            friend class Singleton<AIEntityBuilder>;

            std::shared_ptr<AIObject> buildAIObject(std::string, const std::shared_ptr<const CollisionShape3D> &, const Transform<float> &);
            std::shared_ptr<AITerrain> buildAITerrain(std::string, const std::shared_ptr<const CollisionShape3D> &, const Transform<float> &);

        private:
            std::shared_ptr<AITerrain> buildAITerrain(std::string, const std::shared_ptr<const CollisionHeightfieldShape> &, const Transform<float> &);
    };

}

#endif
