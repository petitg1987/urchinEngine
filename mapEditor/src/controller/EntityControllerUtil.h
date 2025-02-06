#pragma once

#include <list>
#include <regex>
#include <UrchinAggregation.h>

namespace urchin {

    class EntityControllerUtil {
        public:
            static Point3<float> determineNewPosition(const Point3<float>&, bool, const Camera& camera);
            static Point3<float> computeFrontCameraPosition(const Camera& camera);
            template<class T> static std::string determineNewCloneName(const std::string&, const std::list<const T*>&);

        private:
            static constexpr float ENTITY_NEAR_TO_CAMERA_DISTANCE = 50.0f;
    };

    #include "EntityControllerUtil.inl"

}
