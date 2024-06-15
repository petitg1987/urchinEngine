#pragma once

#include <vector>
#include <mutex>
#include <UrchinCommon.h>

#include <graphics/api/GraphicsApi.h>

namespace urchin {

    struct GrassInstanceData {
        Point3<float> grassPosition;
        Vector3<float> grassNormal;
    };

    class TerrainGrassQuadtree {
        public:
            TerrainGrassQuadtree() = default;
            explicit TerrainGrassQuadtree(std::vector<std::unique_ptr<TerrainGrassQuadtree>>);

            void setRenderer(std::unique_ptr<GenericRenderer>);
            GenericRenderer* getRenderer() const;

            bool isLeaf() const;
            const AABBox<float>* getBox() const;

            void addChild(std::unique_ptr<TerrainGrassQuadtree>);
            const std::vector<std::unique_ptr<TerrainGrassQuadtree>>& getChildren() const;

            void addGrassInstanceData(const Point3<float>&, const Vector3<float>&);
            const std::vector<GrassInstanceData>& getGrassInstanceData() const;

        private:
            std::unique_ptr<GenericRenderer> renderer;

            std::mutex mutexAddVertex;
            std::vector<std::unique_ptr<TerrainGrassQuadtree>> children;

            mutable std::unique_ptr<AABBox<float>> bbox;
            std::vector<GrassInstanceData> grassInstanceData;
    };

}
