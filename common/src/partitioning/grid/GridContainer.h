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

    template<class T> using ItemSet = std::set<std::shared_ptr<T>, AxisCompare<std::shared_ptr<T>>>;

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

            void addItem(std::shared_ptr<T>);
            void removeItem(const Point3<int>&);

            bool isItemExist(const Point3<int>&) const;
            void getItems(std::vector<T*>&) const;
            std::shared_ptr<T> findNeighbor(const Point3<int>&, Axis, Direction) const;

        private:
            std::int64_t buildKey(const Point3<int>&, std::size_t) const;
            typename ItemSet<T>::iterator findInItemSet(const ItemSet<T>&, const Point3<int>&, std::size_t) const;

            std::unordered_map<std::int64_t, ItemSet<T>> axisSortedItems[3];
    };

    #include "GridContainer.inl"

}
