#include <utility>

#include "AITerrain.h"

namespace urchin {
    AITerrain::AITerrain(std::string name, const Transform<float> &transform, bool bIsObstacleCandidate,
                         std::vector<Point3<float>> localVertices, unsigned int xLength, unsigned int zLength) :
            AIEntity(std::move(name), transform, bIsObstacleCandidate),
            localVertices(std::move(localVertices)), xLength(xLength), zLength(zLength) {

    }

    AIEntity::AIEntityType AITerrain::getType() const {
        return AIEntity::TERRAIN;
    }

    const std::vector<Point3<float>> &AITerrain::getLocalVertices() const {
        return localVertices;
    }

    unsigned int AITerrain::getXLength() const {
        return xLength;
    }

    unsigned int AITerrain::getZLength() const {
        return zLength;
    }
}
