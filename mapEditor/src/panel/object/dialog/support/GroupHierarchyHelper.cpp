#include <ranges>

#include "panel/object/dialog/support/GroupHierarchyHelper.h"

namespace urchin {

    std::vector<std::vector<std::string>> GroupHierarchyHelper::getAllGroupHierarchy(const ObjectController& objectController) {
        std::map<std::string, std::vector<std::string>> allGroupHierarchyMap;
        std::list<const ObjectEntity*> allObjectEntities = objectController.getObjectEntities();
        for (const ObjectEntity* objectEntity : allObjectEntities) {
            const std::vector<std::string>& groupHierarchy = objectEntity->getGroupHierarchy();

            std::vector<std::string> currentGroup;
            for (std::string groupName : groupHierarchy) {
                currentGroup.push_back(groupName);
                std::string key = StringUtil::join(currentGroup, '#');
                allGroupHierarchyMap.insert({key, currentGroup});
            }
        }

        std::vector<std::vector<std::string>> allGroupHierarchy;
        allGroupHierarchy.reserve(allGroupHierarchyMap.size() + 1);
        allGroupHierarchy.push_back({}); //root
        for (const std::vector<std::string>& groupHierarchy : std::views::values(allGroupHierarchyMap)) {
            allGroupHierarchy.push_back(groupHierarchy);
        }

        return allGroupHierarchy;
    }

    std::string GroupHierarchyHelper::groupHierarchyToString(const std::vector<std::string>& groupHierarchy) {
        if (groupHierarchy.size() == 0) {
            return ROOT_TEXT;
        }
        return StringUtil::join(groupHierarchy, GROUP_DELIMITER);
    }

    std::vector<std::string> GroupHierarchyHelper::stringToGroupHierarchy(const std::string& groupHierarchyString) {
        if (groupHierarchyString.empty() || groupHierarchyString == ROOT_TEXT) {
            return {};
        }
        return StringUtil::split(groupHierarchyString, GROUP_DELIMITER);
    }

}
