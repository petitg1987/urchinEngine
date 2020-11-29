#include "TerrainSplitService.h"

namespace urchin {
    TerrainSplitService::TerrainSplitService(float terrainMaxSize) :
            terrainMaxSize(terrainMaxSize) {

    }

    std::vector<TerrainSplit> TerrainSplitService::splitTerrain(const std::string &name, const Point3<float>& position,
            const std::vector<Point3<float>> &localVertices, unsigned int xLength, unsigned int zLength) const {
        std::vector<TerrainSplit> terrainSplits;

        Point3<float> farLeftVertex = position + localVertices[0];
        Point3<float> nearRightVertex = position + localVertices[(xLength * zLength) - 1];

        float terrainXSize = nearRightVertex.X - farLeftVertex.X;
        auto xSamples = static_cast<unsigned int>(std::ceil(terrainXSize / terrainMaxSize));
        float terrainZSize = nearRightVertex.Z - farLeftVertex.Z;
        auto zSamples = static_cast<unsigned int>(std::ceil(terrainZSize / terrainMaxSize));

        unsigned int zShit = 0;
        for (unsigned int zSample=0; zSample < zSamples; ++zSample) {
            unsigned int zSize = std::lround((float)((zSample + 1) * zLength) / (float)zSamples) - zShit;
            unsigned int xShit = 0;

            for (unsigned int xSample=0; xSample < xSamples; ++xSample) {
                unsigned int xSize = std::lround((float)((xSample + 1) * xLength) / (float)xSamples) - xShit;

                TerrainSplit terrainSplit = TerrainSplit();
                terrainSplit.name = name + "-" + std::to_string((zSample * xSamples) + xSample);
                terrainSplit.position = position;
                terrainSplit.localVertices.reserve(xSize * zSize);
                terrainSplit.xLength = xSize;
                terrainSplit.zLength = zSize;
                for (unsigned int z=0; z < zSize; ++z) {
                    for (unsigned int x=0; x < xSize; ++x) {
                        unsigned int verticesPos = ((zShit + z) * xLength) + xShit + x;
                        terrainSplit.localVertices.emplace_back(localVertices[verticesPos]);
                    }
                }
                terrainSplits.push_back(terrainSplit);

                xShit += xSize - 1;
            }

            zShit += zSize - 1;
        }

        return terrainSplits;
    }
}
