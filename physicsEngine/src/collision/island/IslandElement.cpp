#include "IslandElement.h"

namespace urchin {

    IslandElement::IslandElement() :
            islandElementId(0) {

    }

    IslandElement::IslandElement(const IslandElement& islandElement) :
            islandElementId(islandElement.getIslandElementId()) {

    }

    void IslandElement::setIslandElementId(unsigned int islandElementId) {
        this->islandElementId = islandElementId;
    }

    unsigned int IslandElement::getIslandElementId() const {
        return islandElementId;
    }

}

