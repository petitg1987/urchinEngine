#pragma once

#include <cstdlib>
#include <atomic>

namespace urchin {

    /**
     * Fifo queue designed to be used by exactly one producer and one consumer
     */
    template<class T, std::size_t Size> class CircularFifo {
        public:
            enum { Capacity = Size + 1 };

            CircularFifo();

            bool push(T);
            bool pop(T&);

        private:
            std::size_t increment(std::size_t) const;

            std::atomic<std::size_t> head;
            std::atomic<std::size_t> tail;
            std::array<T, Capacity> array;
    };

    #include "CircularFifo.inl"

}
