#include <stdexcept>

#include "TerrainController.h"

namespace urchin
{
    TerrainController::TerrainController(MapHandler *mapHandler) :
            bIsModified(false),
            mapHandler(mapHandler)
    {

    }

    bool TerrainController::isModified() const
    {
        return bIsModified;
    }

    void TerrainController::markModified()
    {
        bIsModified = true;
    }

    void TerrainController::resetModified()
    {
        bIsModified = false;
    }

    std::list<const SceneTerrain *> TerrainController::getSceneTerrains() const
    {
        const std::list<SceneTerrain *> &sceneTerrains = mapHandler->getMap()->getSceneTerrains();
        std::list<const SceneTerrain *> constSceneTerrains;
        constSceneTerrains.insert(constSceneTerrains.begin(), sceneTerrains.begin(), sceneTerrains.end());

        return constSceneTerrains;
    }

    void TerrainController::addSceneTerrain(SceneTerrain *sceneTerrain)
    {
        mapHandler->getMap()->addSceneTerrain(sceneTerrain);

        markModified();
    }

    void TerrainController::removeSceneTerrain(const SceneTerrain *constSceneTerrain)
    {
        SceneTerrain *sceneTerrain = findSceneTerrain(constSceneTerrain);
        mapHandler->getMap()->removeSceneTerrain(sceneTerrain);

        markModified();
    }

    SceneTerrain *TerrainController::findSceneTerrain(const SceneTerrain *constSceneTerrain)
    {
        const std::list<SceneTerrain *> &sceneTerrains = mapHandler->getMap()->getSceneTerrains();
        auto it = std::find(sceneTerrains.begin(), sceneTerrains.end(), constSceneTerrain);

        if(it!=sceneTerrains.end())
        {
            return *it;
        }

        throw std::invalid_argument("Impossible to find scene terrain: " + constSceneTerrain->getName());
    }
}
