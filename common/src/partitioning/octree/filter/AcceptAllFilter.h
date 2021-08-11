#pragma once

#include <partitioning//octree//filter/OctreeableFilter.h>

namespace urchin {

    template<class T> class AcceptAllFilter : public OctreeableFilter<T> {
        public:
            bool isAccepted(const T*, const ConvexObject3D<float>&) const override;
    };

    #include "AcceptAllFilter.inl"

}
