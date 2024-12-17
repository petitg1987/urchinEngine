#pragma once

#include <cstddef>
#include <vector>
#include <list>

namespace urchin {

    /**
     * Similar to std::unordered_map but the memory is not shrink after a clear or an erase.
     * The purpose is to avoid memory re-allocation on 'insert' occurring after a 'clear'.
     */
    template <typename K, typename V> class EverGrowHashMap {
        public:
            explicit EverGrowHashMap(std::size_t = 8, float = 0.75);

            void insert(const K&, const V&);
            V at(const K&) const;
            V* find(const K&);
            bool erase(const K&);
            void clear();

            bool isEmpty() const;
            std::size_t getSize() const;
            std::size_t getNumBuckets() const;

        private:
            struct Node {
                K key;
                V value;
                Node(const K&, const V&);
            };

            void rehash();

            std::vector<std::vector<Node>> table;
            std::size_t numBuckets;
            std::size_t currentSize;
            float maxLoadFactor;
            std::hash<K> hashFunc;
    };

    #include "EverGrowHashMap.inl"

}
