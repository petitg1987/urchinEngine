#ifndef URCHINENGINE_OCTREEDISPLAYER_H
#define URCHINENGINE_OCTREEDISPLAYER_H

#include "scene/renderer3d/octree/OctreeManager.h"

namespace urchin
{

    template<class T> class OctreeDisplayer
    {
        public:
            explicit OctreeDisplayer(const OctreeManager<T> *);

            void drawOctree(const Matrix4<float> &, const Matrix4<float> &) const;

        private:
            const OctreeManager<T> *octreeManager;
    };

    #include "OctreeDisplayer.inl"

}

#endif
