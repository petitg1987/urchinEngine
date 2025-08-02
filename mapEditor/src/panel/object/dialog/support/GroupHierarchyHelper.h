#pragma once

#include <vector>
#include <string>

#include "controller/object/ObjectController.h"

namespace urchin {

    class GroupHierarchyHelper {
        public:
            static std::vector<std::vector<std::string>> getAllGroupHierarchy(const ObjectController&);

            static std::string groupHierarchyToString(const std::vector<std::string>&);
            static std::vector<std::string> stringToGroupHierarchy(const std::string&);

        private:
            static constexpr std::string GROUP_DELIMITER = " > ";
            static constexpr std::string ROOT_TEXT = "(root)";
    };

}
