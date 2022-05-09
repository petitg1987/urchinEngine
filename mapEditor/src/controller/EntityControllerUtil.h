#pragma once

#include <UrchinAggregation.h>

namespace urchin {

    class EntityControllerUtil {
        public:
            static Point3<float> determineClonePosition(const Point3<float>&, bool, const Camera& camera);

        private:
            static constexpr float ENTITY_NEAR_TO_CAMERA_DISTANCE = 50.0f;
    };

}
