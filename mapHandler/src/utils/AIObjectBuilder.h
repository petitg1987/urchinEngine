#ifndef URCHINENGINE_AIOBJECTBUILDER_H
#define URCHINENGINE_AIOBJECTBUILDER_H

#include <memory>
#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"
#include "UrchinAIEngine.h"

namespace urchin
{

    class AIObjectBuilder : public Singleton<AIObjectBuilder>
    {
        public:
            friend class Singleton<AIObjectBuilder>;

            std::shared_ptr<AIObject> buildAIObject(std::string, const std::shared_ptr<const CollisionShape3D> &, const Transform<float> &);

        private:
            std::shared_ptr<AIObject> heightfieldToAIObject(std::string, const std::shared_ptr<const CollisionHeightfieldShape> &, const Transform<float> &);
            std::vector<Point3<float>> getHeightfieldVertices(const std::shared_ptr<const CollisionHeightfieldShape> &, unsigned int, unsigned int, unsigned int, unsigned int);
    };

}

#endif
