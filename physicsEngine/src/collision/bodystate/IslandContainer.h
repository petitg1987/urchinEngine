#pragma once

#include <vector>

#include <collision/bodystate/IslandElementLink.h>

namespace urchin {

    /**
    * Island container. An island is a set of elements/bodies which are in contact.
    * The island are useful to made sleep elements/bodies. If all elements/bodies of an island doesn't
    * move: there is no need to execute collision detection checks on these island elements/bodies.
    */
    class IslandContainer {
        public:
            IslandContainer();

            void reset(const std::vector<IslandElement*>&);
            void mergeIsland(const IslandElement&, const IslandElement&);
            void linkToStaticElement(const IslandElement&);

            const std::vector<IslandElementLink>& retrieveSortedIslandElements();

        private:
            unsigned int findIslandId(unsigned int) const;

            std::vector<IslandElementLink> islandElementsLink;

            bool containerSorted;
    };

}
