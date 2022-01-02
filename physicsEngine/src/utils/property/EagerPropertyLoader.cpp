#include <UrchinCommon.h>

#include <utils/property/EagerPropertyLoader.h>

namespace urchin {

    float EagerPropertyLoader::getCollisionShapeInnerMargin() {
        static float collisionShapeInnerMargin = ConfigService::instance().getFloatValue("collisionShape.innerMargin");
        return collisionShapeInnerMargin;
    }

}
