#pragma once

#include <vector>
#include <UrchinCommon.h>

namespace urchin {

    struct TerrainSplit {
        std::string name;
        Point3<float> position;
        std::vector<Point3<float>> localVertices;
        unsigned int xLength;
        unsigned int zLength;
    };

    class TerrainSplitService {
        public:
            explicit TerrainSplitService(float);

            std::vector<TerrainSplit> splitTerrain(const std::string&, const Point3<float>&, const std::vector<Point3<float>>&, unsigned int, unsigned int) const;

        private:
            float terrainMaxSize;
    };

}
