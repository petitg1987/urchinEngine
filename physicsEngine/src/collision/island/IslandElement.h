#ifndef URCHINENGINE_ISLANDELEMENT_H
#define URCHINENGINE_ISLANDELEMENT_H

namespace urchin {

    /**
    * An element which can belong to an island.
    */
    class IslandElement {
        public:
            IslandElement();
            virtual ~IslandElement() = default;

            void setIslandElementId(unsigned int);
            unsigned int getIslandElementId() const;

            virtual bool isActive() const = 0;

        private:
            unsigned int islandElementId;
    };

}

#endif
