#pragma once

#include <vector>
#include <mutex>
#include <UrchinCommon.h>

#include <graphic/render/GenericRenderer.h>

namespace urchin {

    class TerrainGrassQuadtree {
        public:
            TerrainGrassQuadtree() = default;
            explicit TerrainGrassQuadtree(std::vector<std::unique_ptr<TerrainGrassQuadtree>>);

            void setRenderer(std::unique_ptr<GenericRenderer>);
            GenericRenderer* getRenderer() const;

            bool isLeaf() const;
            const std::unique_ptr<AABBox<float>>& getBox() const;

            void addChild(std::unique_ptr<TerrainGrassQuadtree>);
            const std::vector<std::unique_ptr<TerrainGrassQuadtree>>& getChildren() const;

            void addVertex(const Point3<float>&, const Vector3<float>&);
            const std::vector<Point3<float>>& getGrassVertices() const;
            const std::vector<Vector3<float>>& getGrassNormals() const;

        private:
            std::unique_ptr<GenericRenderer> renderer;

            std::mutex mutexAddVertex;
            std::vector<std::unique_ptr<TerrainGrassQuadtree>> children;

            mutable std::unique_ptr<AABBox<float>> bbox;
            std::vector<Point3<float>> grassVertices;
            std::vector<Vector3<float>> normals;
    };

}
