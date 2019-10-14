#ifndef URCHINENGINE_TERRAINGRASSQUADTREE_H
#define URCHINENGINE_TERRAINGRASSQUADTREE_H

#include <vector>
#include <mutex>
#include "UrchinCommon.h"

namespace urchin
{

    class TerrainGrassQuadtree
    {
        public:
            TerrainGrassQuadtree();
            explicit TerrainGrassQuadtree(std::vector<TerrainGrassQuadtree *> );
            ~TerrainGrassQuadtree();

            void setVertexArrayObjectId(unsigned int);
            unsigned int getVertexArrayObjectId() const;

            bool isLeaf() const;
            const std::unique_ptr<AABBox<float>> &getBox() const;

            void addChild(TerrainGrassQuadtree *);
            const std::vector<TerrainGrassQuadtree *> &getChildren() const;

            void addVertex(const Point3<float> &, const Vector3<float> &);
            const std::vector<Point3<float>> &getGrassVertices() const;
            const std::vector<Vector3<float>> &getGrassNormals() const;

        private:
            unsigned int vertexArrayObjectId;

            std::mutex mutexAddVertex;
            std::vector<TerrainGrassQuadtree *> children;

            mutable std::unique_ptr<AABBox<float>> bbox;
            std::vector<Point3<float>> grassVertices;
            std::vector<Vector3<float>> normals;
    };

}

#endif
