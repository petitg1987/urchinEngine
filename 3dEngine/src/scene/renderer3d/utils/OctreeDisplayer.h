#ifndef URCHINENGINE_OCTREEDISPLAYER_H
#define URCHINENGINE_OCTREEDISPLAYER_H

#include "UrchinCommon.h"

#include "graphic/displayer/geometry/aabbox/AABBoxModel.h"

namespace urchin {

    class OctreeDisplayer {
        public:
            template<class T> static void drawOctree(const OctreeManager<T> *, const Matrix4<float> &, const Matrix4<float> &);
    };

    #include "OctreeDisplayer.inl"

}

#endif
