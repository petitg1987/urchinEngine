#pragma once

#include <vector>
#include <string>

#include "controller/object/ObjectController.h"

namespace urchin {

    class GroupHierarchyHelper {
        public:
            static std::vector<std::vector<std::string>> getAllGroupHierarchy(const ObjectController&);

            static constexpr std::string GROUP_DELIMITER = " > ";
    };

}
