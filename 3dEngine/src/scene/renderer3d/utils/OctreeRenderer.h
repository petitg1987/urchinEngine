#ifndef URCHINENGINE_OCTREERENDERER_H
#define URCHINENGINE_OCTREERENDERER_H

#include "UrchinCommon.h"

#include "utils/geometry/aabbox/AABBoxModel.h"

namespace urchin {

    class OctreeRenderer {
        public:
            template<class T> static void drawOctree(const OctreeManager<T> *, const Matrix4<float> &, const Matrix4<float> &);
    };

    #include "OctreeRenderer.inl"

}

#endif
