#pragma once

#include <set>
#include <unordered_map>
#include <utility>
#include <cstdint>

#include <util/VectorUtil.h>

namespace urchin {

    template<class T> class AxisCompare {
        public:
            explicit AxisCompare(std::size_t);

            bool operator() (const T&, const T&) const;

        private:
            std::size_t axisIndex;
    };

    template<class T> class GridContainer {
        public:
            enum Axis {
                X = 0,
                Y = 1,
                Z = 2
            };
            enum Direction {
                POSITIVE,
                NEGATIVE
            };

            void addItem(T*);
            void removeItem(T*);

            T* findNeighbor(const Point3<int>&, Axis, Direction) const;

        private:
            std::int64_t buildKey(const Point3<int>&, std::size_t) const;

            std::unordered_map<std::int64_t, std::set<T*, AxisCompare<T*>>> axisSortedItems[3];
    };

    #include "GridContainer.inl"

}
