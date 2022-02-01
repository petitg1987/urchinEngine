#pragma once

#include <vector>

namespace urchin {

    template<class T> class OctreeableHelper {
        public:
            OctreeableHelper() = delete;
            ~OctreeableHelper() = delete;

            static void merge(std::vector<T*>&, std::span<T* const>);
    };

    #include "OctreeableHelper.inl"

}
