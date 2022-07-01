#pragma once

namespace urchin {

    template<class T> class AcceptAllFilter {
        public:
            bool isAccepted(const T*, const ConvexObject3D<float>&) const;
    };

    #include "AcceptAllFilter.inl"

}
