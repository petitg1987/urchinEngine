#ifndef URCHINENGINE_TERRAINSPLITSERVICE_H
#define URCHINENGINE_TERRAINSPLITSERVICE_H

#include <vector>
#include "UrchinCommon.h"

#include "input/AITerrain.h"

namespace urchin
{

    struct TerrainSplit
    {
        std::string name;
        Point3<float> position;
        std::vector<Point3<float>> localVertices;
        unsigned int xLength, zLength;
    };

    class TerrainSplitService
    {
        public:
            explicit TerrainSplitService(float);

            std::vector<TerrainSplit> splitTerrain(const std::shared_ptr<AITerrain> &aiTerrain) const;

        private:
            float terrainMaxSize;
    };

}

#endif
