#pragma once

#include <cstdlib>
#include <atomic>

namespace urchin {

    template<class T, std::size_t Size> class ProdConsCircular {
        public:
            enum { Capacity = Size + 1 };

            ProdConsCircular();

            bool push(T);
            bool pop(T&);

        private:
            std::size_t increment(std::size_t) const;

            std::atomic<std::size_t> head;
            std::atomic<std::size_t> tail;
            T array[Capacity];
    };

    #include "ProdConsCircular.inl"

}
