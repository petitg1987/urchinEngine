#ifndef GREENCITY_TERRAINGRASSQUADTREE_H
#define GREENCITY_TERRAINGRASSQUADTREE_H

#include <vector>
#include "UrchinCommon.h"

namespace urchin
{

    class TerrainGrassQuadtree
    {
        public:
            TerrainGrassQuadtree() = default;
            explicit TerrainGrassQuadtree(const std::vector<TerrainGrassQuadtree *> &);
            ~TerrainGrassQuadtree();

            bool isLeaf() const;
            const std::unique_ptr<AABBox<float>> &getBox() const;

            void addChild(TerrainGrassQuadtree *);
            const std::vector<TerrainGrassQuadtree *> &getChildren() const;

            void addVertex(const Point3<float> &, const Vector3<float> &);
            const std::vector<Point3<float>> &getGrassVertices() const;
            const std::vector<Vector3<float>> &getGrassNormals() const;

        private:
            std::vector<TerrainGrassQuadtree *> children;

            mutable std::unique_ptr<AABBox<float>> bbox;
            std::vector<Point3<float>> grassVertices;
            std::vector<Vector3<float>> normals;
    };

}

#endif
