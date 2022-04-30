#pragma once

#include <cstdlib>
#include <atomic>

namespace urchin {

    template<class Element, std::size_t Size> class ProdConsCircular {
        public:
            enum { Capacity = Size+1 };

            ProdConsCircular();

            bool push(Element);
            bool pop(Element&);

        private:
            std::size_t increment(std::size_t) const;

            std::atomic<std::size_t> head;
            std::atomic<std::size_t> tail;
            Element array[Capacity];
    };

    #include "ProdConsCircular.inl"

}
