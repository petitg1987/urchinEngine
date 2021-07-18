#include <collision/bodystate/IslandElementLink.h>

namespace urchin {

    IslandElementLink::IslandElementLink() :
        element(nullptr),
        islandIdRef(0),
        linkedToStaticElement(false) {

    }

    bool IslandElementLinkSortPredicate::operator()(const IslandElementLink& elem1, const IslandElementLink& elem2) const {
        return elem1.islandIdRef < elem2.islandIdRef;
    }

}
