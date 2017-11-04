#include <algorithm>

#include "TerrainDisplayer.h"

namespace urchin
{

    void TerrainDisplayer::addTerrain(Terrain *terrain)
    {
        terrains.push_back(terrain);
    }

    void TerrainDisplayer::removeTerrain(Terrain *terrain)
    {
        terrains.erase(std::remove(terrains.begin(), terrains.end(), terrain), terrains.end());
    }

    void TerrainDisplayer::onCameraProjectionUpdate(const Camera *camera)
    {
        for(const auto terrain : terrains)
        {
            terrain->onCameraProjectionUpdate(camera->getProjectionMatrix());
        }
    }

    void TerrainDisplayer::display(const Matrix4<float> &viewMatrix) const
    {
        glEnable(GL_PRIMITIVE_RESTART);
        glPrimitiveRestartIndex(RESTART_INDEX);

        for(const auto terrain : terrains)
        {
            terrain->display(viewMatrix);
        }

        glDisable(GL_PRIMITIVE_RESTART);
    }
}
