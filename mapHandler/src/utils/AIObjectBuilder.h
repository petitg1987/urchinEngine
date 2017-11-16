#ifndef URCHINENGINE_AIOBJECTBUILDER_H
#define URCHINENGINE_AIOBJECTBUILDER_H

#include <memory>
#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"
#include "UrchinAIEngine.h"

namespace urchin
{

    class AIObjectBuilder
    {
        public:
            static std::shared_ptr<AIObject> buildAIObject(std::string, const std::shared_ptr<const CollisionShape3D> &, const Transform<float> &);
    };

}

#endif
