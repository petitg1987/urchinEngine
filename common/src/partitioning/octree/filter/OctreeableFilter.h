#pragma once

namespace urchin {

    template<class T> class OctreeableFilter {
        public:
            virtual ~OctreeableFilter() = default;

            virtual bool isAccepted(const T*, const ConvexObject3D<float>&) const = 0;
    };

}
