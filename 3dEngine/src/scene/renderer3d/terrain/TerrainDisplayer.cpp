#include <algorithm>

#include "TerrainDisplayer.h"

namespace urchin
{

    void TerrainDisplayer::addTerrain(Terrain *terrain)
    {
        terrains.push_back(terrain);

        terrain->onCameraProjectionUpdate(projectionMatrix);
    }

    void TerrainDisplayer::removeTerrain(Terrain *terrain)
    {
        terrains.erase(std::remove(terrains.begin(), terrains.end(), terrain), terrains.end());
    }

    void TerrainDisplayer::onCameraProjectionUpdate(const Camera *camera)
    {
        this->projectionMatrix = camera->getProjectionMatrix();
        for(const auto terrain : terrains)
        {
            terrain->onCameraProjectionUpdate(projectionMatrix);
        }
    }

    void TerrainDisplayer::display(const Camera *camera, float invFrameRate) const
    {
        glEnable(GL_PRIMITIVE_RESTART);
        glPrimitiveRestartIndex(RESTART_INDEX);

        for(const auto terrain : terrains)
        {
            terrain->display(camera, invFrameRate);
        }

        glDisable(GL_PRIMITIVE_RESTART);
    }
}
