#ifndef URCHINENGINE_OCTREEABLEHELPER_H
#define URCHINENGINE_OCTREEABLEHELPER_H

#include <vector>

namespace urchin {

    template<class TOctreeable> class OctreeableHelper {
        public:
            void merge(std::vector<TOctreeable*>&, const std::vector<TOctreeable*>&);
    };

    #include "OctreeableHelper.inl"

}

#endif
