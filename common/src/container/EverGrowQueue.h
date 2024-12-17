#pragma once

#include <vector>
#include <cassert>

namespace urchin {

    /**
     * Similar to std::deque but the memory is not shrink after a pop.
     * The purpose is to avoid memory re-allocation on 'push' occurring after a 'pop'.
     */
    template<class T> class EverGrowQueue {
        public:
            EverGrowQueue(int, int);

            void pushFront(const T&);
            void pushBack(const T&);
            T popFront();
            T popBack();

            bool isEmpty() const;
            int getSize() const;
            int getCapacity() const;

        private:
            void expandCapacity();

            std::vector<T> data;
            int capacity;
            int size;
            int frontIndex;
            int backIndex;
            const int chunkSize;
    };

    #include "EverGrowQueue.inl"

}