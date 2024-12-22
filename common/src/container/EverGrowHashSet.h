#pragma once

#include <cstddef>
#include <vector>

namespace urchin {

    /**
     * Similar to std::unordered_set but the memory is not shrink after a clear.
     * The purpose is to avoid memory re-allocation on 'insert' occurring after a 'clear'.
     */
    template <typename K> class EverGrowHashSet {
        public:
            class Iterator {
                public:
                    Iterator(EverGrowHashSet&, std::size_t, std::size_t);

                    K& operator*() const;
                    K* operator->() const;
                    Iterator& operator++();
                    bool operator==(const Iterator&) const;
                    bool operator!=(const Iterator&) const;

                private:
                    EverGrowHashSet& set;
                    std::size_t bucketIndex;
                    std::size_t elementIndex;
            };

            explicit EverGrowHashSet(std::size_t = 8, float = 0.75);

            bool insert(const K&);
            bool isExist(const K&);
            bool erase(const K&);
            void clear();

            Iterator begin();
            Iterator end();

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
