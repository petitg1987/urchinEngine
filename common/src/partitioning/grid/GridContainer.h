#pragma once

#include <vector>
#include <unordered_map>
#include <utility>
#include <cstdint>

#include <util/VectorUtil.h>

namespace urchin {

    template<class T> class GridContainer {
        public:
            enum NeighborDirection {
                X_POSITIVE,
                X_NEGATIVE,
                Y_POSITIVE,
                Y_NEGATIVE,
                Z_POSITIVE,
                Z_NEGATIVE
            };

            void addItem(T*);
            void removeItem(T*);

            T* findNeighbor(T*, NeighborDirection) const;

        private:
            std::unordered_map<std::int64_t, std::vector<T*>> xSortedItems;
            std::unordered_map<std::int64_t, std::vector<T*>> ySortedItems;
            std::unordered_map<std::int64_t, std::vector<T*>> zSortedItems;
    };

    #include "GridContainer.inl"

}
