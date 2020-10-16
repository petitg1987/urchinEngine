#ifndef URCHINENGINE_ACCEPTALLFILTER_H
#define URCHINENGINE_ACCEPTALLFILTER_H

#include "OctreeableFilter.h"

namespace urchin {

    template<class TOctreeable> class AcceptAllFilter : public OctreeableFilter<TOctreeable> {
        public:
            ~AcceptAllFilter() override = default;

            bool isAccepted(const TOctreeable *, const ConvexObject3D<float> &) const;
    };

    #include "AcceptAllFilter.inl"

}

#endif
