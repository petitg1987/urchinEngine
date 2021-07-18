#pragma once

namespace urchin {

    /**
    * An element which can belong to an island.
    */
    class IslandElement {
        public:
            IslandElement();
            IslandElement(const IslandElement&);
            virtual ~IslandElement() = default;

            void setIslandElementId(unsigned int);
            unsigned int getIslandElementId() const;

            virtual bool isActive() const = 0;

        private:
            unsigned int islandElementId;
    };

}
