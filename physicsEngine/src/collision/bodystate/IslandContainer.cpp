#include <algorithm>
#include <cassert>

#include <collision/bodystate/IslandContainer.h>

namespace urchin {

    IslandContainer::IslandContainer() :
        containerSorted(false) {

    }

    /**
     * Reset the container of island elements. Create islands of one element for each island elements asked.
     */
    void IslandContainer::reset(const std::vector<IslandElement*>& islandElements) {
        containerSorted = false;

        islandElementsLink.clear();
        islandElementsLink.resize(islandElements.size());

        for (std::size_t i = 0; i < islandElements.size(); ++i) {
            islandElements[i]->setIslandElementId((unsigned int)i);

            islandElementsLink[i].element = islandElements[i];
            islandElementsLink[i].linkedToStaticElement = !islandElements[i]->isActive();
            islandElementsLink[i].islandIdRef = (unsigned int)i;
        }
    }

    void IslandContainer::mergeIsland(IslandElement& element1, IslandElement& element2) {
        assert(!containerSorted);

        unsigned int islandId1 = findIslandId(element1.getIslandElementId());
        unsigned int islandId2 = findIslandId(element2.getIslandElementId());

        if (islandId1 == islandId2) { //elements are already in the same island
            return;
        }

        islandElementsLink[islandId1].islandIdRef = islandId2;
    }

    void IslandContainer::linkToStaticElement(IslandElement& element) {
        unsigned int islandId = findIslandId(element.getIslandElementId());
        islandElementsLink[islandId].linkedToStaticElement = true;
    }

    /**
     * Sorts the islands by ID and returns them.
     * Once the islands sorted, the container is not usable anymore and need to be reset.
     */
    const std::vector<IslandElementLink>& IslandContainer::retrieveSortedIslandElements() {
        //store directly island ID on islandIdRef instead of reference
        for (std::size_t i = 0; i < islandElementsLink.size(); ++i) {
            islandElementsLink[i].islandIdRef = findIslandId((unsigned int)i);
        }

        std::sort(islandElementsLink.begin(), islandElementsLink.end(), IslandElementLinkSortPredicate());

        containerSorted = true;

        return islandElementsLink;
    }

    unsigned int IslandContainer::findIslandId(unsigned int elementRef) const {
        while (elementRef != islandElementsLink[elementRef].islandIdRef) {
            elementRef = islandElementsLink[elementRef].islandIdRef;
        }

        return elementRef;
    }

}
