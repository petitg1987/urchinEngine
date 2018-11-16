#ifndef URCHINENGINE_OCTREEABLEHELPER_H
#define URCHINENGINE_OCTREEABLEHELPER_H

#include <vector>

namespace urchin
{

    template<class TOctreeable> class OctreeableHelper
    {
        public:
            OctreeableHelper() = delete;
            ~OctreeableHelper() = delete;

            static void merge(std::vector<TOctreeable *> &, const std::vector<TOctreeable *> &);
    };

    #include "OctreeableHelper.inl"

}

#endif
