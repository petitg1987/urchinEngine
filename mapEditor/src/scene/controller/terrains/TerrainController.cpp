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

    const SceneTerrain *TerrainController::updateSceneTerrainGeneralProperties(const SceneTerrain *constSceneTerrain,
                                                                               const Point3<float> &position, float ambient)
    {
        SceneTerrain *sceneTerrain = findSceneTerrain(constSceneTerrain);
        Terrain *terrain = sceneTerrain->getTerrain();

        terrain->setPosition(position);
        if(sceneTerrain->getRigidBody()!=nullptr)
        {
            sceneTerrain->getRigidBody()->setTransform(Transform<float>(position));
        }

        terrain->setAmbient(ambient);

        markModified();
        return sceneTerrain;
    }

    const SceneTerrain *TerrainController::updateSceneTerrainMesh(const SceneTerrain *constSceneTerrain, float xzScale, float yScale)
    {
        SceneTerrain *sceneTerrain = findSceneTerrain(constSceneTerrain);
        Terrain *terrain = sceneTerrain->getTerrain();

        auto terrainMesh = std::make_shared<TerrainMesh>(terrain->getMesh()->getHeightFilename(), xzScale, yScale);
        terrain->setMesh(terrainMesh);

        markModified();
        return sceneTerrain;
    }

    const SceneTerrain *TerrainController::updateSceneTerrainMaterial(const SceneTerrain *constSceneTerrain, float sRepeat, float tRepeat, std::string maskMapFilename,
                                                                      const std::vector<std::string> &materialFilenames)
    {
        SceneTerrain *sceneTerrain = findSceneTerrain(constSceneTerrain);
        Terrain *terrain = sceneTerrain->getTerrain();

        auto terrainMaterial = std::make_unique<TerrainMaterial>(maskMapFilename, sRepeat, tRepeat);
        unsigned int i=0;
        for(const auto &materialFilename : materialFilenames)
        {
            if(!materialFilename.empty())
            {
                terrainMaterial->addMaterial(i, materialFilename);
            }
            i++;
        }
        terrain->setMaterial(terrainMaterial);

        markModified();
        return sceneTerrain;
    }

    const SceneTerrain *TerrainController::updateSceneTerrainGrass(const SceneTerrain *constSceneTerrain, std::string grassTextureFilename, std::string grassMaskFilename,
                                                                   unsigned int numGrassInTex, float grassOffset, float grassHeight, float grassLength,
                                                                   const Vector3<float> &windDirection, float windStrength)
    {
        SceneTerrain *sceneTerrain = findSceneTerrain(constSceneTerrain);
        Terrain *terrain = sceneTerrain->getTerrain();

        if(terrain->getGrass()==nullptr)
        {
            auto terrainGrass = std::make_unique<TerrainGrass>(grassTextureFilename);
            terrain->setGrass(terrainGrass);
        } else
        {
            terrain->getGrass()->setGrassTexture(grassTextureFilename);
        }

        TerrainGrass *terrainGrass = terrain->getGrass();

        terrainGrass->setMaskTexture(grassMaskFilename);
        terrainGrass->setNumGrassInTexture(numGrassInTex);
        if(terrainGrass->getGrassOffset()!=grassOffset)
        {
            terrainGrass->setGrassOffset(grassOffset);
        }
        terrainGrass->setGrassHeight(grassHeight);
        terrainGrass->setGrassLength(grassLength);
        terrainGrass->setWindDirection(windDirection);
        terrainGrass->setWindStrength(windStrength);
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
