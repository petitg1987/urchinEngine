#include <ranges>

#include "panel/object/dialog/support/GroupHierarchyHelper.h"

namespace urchin {

    std::vector<std::vector<std::string>> GroupHierarchyHelper::getAllGroupHierarchy(const ObjectController& objectController) { //TODO missing intermediate group !
        std::map<std::string, std::vector<std::string>> allGroupHierarchyMap;
        std::list<const ObjectEntity*> allObjectEntities = objectController.getObjectEntities();
        for (const ObjectEntity* objectEntity : allObjectEntities) {
            const std::vector<std::string>& groupHierarchy = objectEntity->getGroupHierarchy();
            if (!groupHierarchy.empty()) {
                std::string key = StringUtil::join(groupHierarchy, '#');
                allGroupHierarchyMap.insert({key, groupHierarchy});
            }
        }

        std::vector<std::vector<std::string>> allGroupHierarchy;
        allGroupHierarchy.reserve(allGroupHierarchyMap.size());
        for (const std::vector<std::string>& groupHierarchy : std::views::values(allGroupHierarchyMap)) {
            allGroupHierarchy.push_back(groupHierarchy);
        }

        return allGroupHierarchy;
    }

}
