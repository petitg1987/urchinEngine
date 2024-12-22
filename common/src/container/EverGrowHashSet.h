#pragma once

#include <cstddef>
#include <vector>
#include <list>

namespace urchin {

    /**
     * Similar to std::unordered_set but the memory is not shrink after a clear.
     * The purpose is to avoid memory re-allocation on 'insert' occurring after a 'clear'.
     */
    template <typename K> class EverGrowHashSet {
        public:
            explicit EverGrowHashSet(std::size_t = 8, float = 0.75);

            bool insert(const K&);
            bool isExist(const K&);
            bool erase(const K&);
            void clear();

            bool isEmpty() const;
            std::size_t getSize() const;
            std::size_t getNumBuckets() const;

        private:
            void rehash();

            std::vector<std::vector<K>> table;
            std::size_t numBuckets;
            std::size_t currentSize;
            float maxLoadFactor;
            std::hash<K> hashFunc;
    };

    #include "EverGrowHashSet.inl"

}
