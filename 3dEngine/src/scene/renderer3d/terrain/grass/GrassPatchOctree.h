#ifndef GREENCITY_GRASSPATCHOCTREE_H
#define GREENCITY_GRASSPATCHOCTREE_H

#include <vector>
#include "UrchinCommon.h"

namespace urchin
{

    class GrassPatchOctree //TODO rename TerrainGrassOctree ?
    {
        public:
            GrassPatchOctree() = default;
            explicit GrassPatchOctree(const std::vector<GrassPatchOctree *> &);
            ~GrassPatchOctree();

            bool isLeaf() const;
            const std::unique_ptr<AABBox<float>> &getBox() const;

            void addChild(GrassPatchOctree *);
            const std::vector<GrassPatchOctree *> &getChildren() const;

            void addVertex(const Point3<float> &);
            const std::vector<Point3<float>> &getGrassVertices() const;

        private:
            std::vector<GrassPatchOctree *> children;

            mutable std::unique_ptr<AABBox<float>> bbox;
            std::vector<Point3<float>> grassVertices;
    };

}

#endif
