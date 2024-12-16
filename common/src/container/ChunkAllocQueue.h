#pragma once

#include <vector>
#include <cassert>

namespace urchin {

    template<class T> class ChunkAllocQueue {
        public:
            ChunkAllocQueue(int, int);

            void pushFront(const T&);
            void pushBack(const T&);
            T popFront();
            T popBack();

            bool isEmpty() const;
            int getSize() const;
            int getCapacity() const;
            void clear();

        private:
            void expandCapacity();

            std::vector<T> data;
            int capacity;
            int size;
            int frontIndex;
            int backIndex;
            const int chunkSize;
    };

    #include "ChunkAllocQueue.inl"

}