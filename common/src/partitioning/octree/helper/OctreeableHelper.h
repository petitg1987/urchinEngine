#pragma once

#include <vector>

namespace urchin {

    template<class T> class OctreeableHelper {
        public:
            OctreeableHelper() = delete;
            ~OctreeableHelper() = delete;

            static void merge(std::vector<T*>&, const std::vector<T*>&);
    };

    #include "OctreeableHelper.inl"

}
