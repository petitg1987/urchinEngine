#ifndef URCHINENGINE_ISLANDELEMENTLINK_H
#define URCHINENGINE_ISLANDELEMENTLINK_H

#include "collision/island/IslandElement.h"

namespace urchin {

    /**
    * Allow to make the link between an island element and the island ID.
    */
    struct IslandElementLink {
        IslandElementLink();

        IslandElement *element; //reference to the element

        unsigned int islandIdRef; //reference to the next island element. If it references itself: it's the island id.
        bool linkedToStaticElement; //true if 'element' is linked to a static element.
    };

    struct IslandElementLinkSortPredicate {
        bool operator() (const IslandElementLink &, const IslandElementLink &) const;
    };

}

#endif
