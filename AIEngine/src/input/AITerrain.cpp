#include "AITerrain.h"

namespace urchin
{
    AITerrain::AITerrain(std::string name, const Transform<float> &transform, bool bIsObstacleCandidate) :
            AIEntity(std::move(name), transform, bIsObstacleCandidate)
    {

    }

    AIEntity::AIEntityType AITerrain::getType() const
    {
        return AIEntity::TERRAIN;
    }
}
