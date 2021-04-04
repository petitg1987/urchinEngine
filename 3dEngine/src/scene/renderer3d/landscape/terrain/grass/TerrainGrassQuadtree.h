#ifndef URCHINENGINE_TERRAINGRASSQUADTREE_H
#define URCHINENGINE_TERRAINGRASSQUADTREE_H

#include <vector>
#include <mutex>
#include "UrchinCommon.h"

#include "graphic/render/GenericRenderer.h"

namespace urchin {

    class TerrainGrassQuadtree {
        public:
            TerrainGrassQuadtree() = default;
            explicit TerrainGrassQuadtree(std::vector<TerrainGrassQuadtree*> );
            ~TerrainGrassQuadtree();

            void setRenderer(const std::shared_ptr<GenericRenderer>&);
            const std::shared_ptr<GenericRenderer>& getRenderer() const;

            bool isLeaf() const;
            const std::unique_ptr<AABBox<float>>& getBox() const;

            void addChild(TerrainGrassQuadtree*);
            const std::vector<TerrainGrassQuadtree*>& getChildren() const;

            void addVertex(const Point3<float>&, const Vector3<float>&);
            const std::vector<Point3<float>>& getGrassVertices() const;
            const std::vector<Vector3<float>>& getGrassNormals() const;

        private:
            std::shared_ptr<GenericRenderer> renderer;

            std::mutex mutexAddVertex;
            std::vector<TerrainGrassQuadtree*> children;

            mutable std::unique_ptr<AABBox<float>> bbox;
            std::vector<Point3<float>> grassVertices;
            std::vector<Vector3<float>> normals;
    };

}

#endif
