#pragma once

#include <cstddef>
#include <vector>

namespace urchin {

    /**
     * Similar to std::unordered_map but the memory is not shrink after a clear or an erase.
     * The purpose is to avoid memory re-allocation on 'insert' occurring after a 'clear'.
     */
    template <typename K, typename V> class EverGrowHashMap {
        public:
            struct Node {
                K key;
                V value;
                Node(const K&, const V&);
            };

            class Iterator {
                public:
                    Iterator(EverGrowHashMap&, std::size_t, std::size_t);

                    Node& operator*() const;
                    Node* operator->() const;
                    Iterator& operator++();
                    bool operator==(const Iterator&) const;
                    bool operator!=(const Iterator&) const;

                private:
                    EverGrowHashMap& map;
                    std::size_t bucketIndex;
                    std::size_t elementIndex;
            };

            explicit EverGrowHashMap(std::size_t = 8, float = 0.75);

            void insert(const K&, const V&);
            V at(const K&) const;
            V* find(const K&);
            bool erase(const K&);
            void clear();

            Iterator begin();
            Iterator end();

            bool isEmpty() const;
            std::size_t getSize() const;
            std::size_t getNumBuckets() const;

        private:
            void rehash();

            std::vector<std::vector<Node>> table;
            std::size_t numBuckets;
            std::size_t currentSize;
            float maxLoadFactor;
            std::hash<K> hashFunc;
    };

    #include "EverGrowHashMap.inl"

}
