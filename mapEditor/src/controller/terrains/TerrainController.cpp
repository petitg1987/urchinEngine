#include <stdexcept>

#include "TerrainController.h"

namespace urchin {
    TerrainController::TerrainController() :
            AbstractController() {

    }

    std::list<const SceneTerrain*> TerrainController::getSceneTerrains() const {
        const std::list<SceneTerrain*>& sceneTerrains = getMapHandler()->getMap()->getSceneTerrains();
        std::list<const SceneTerrain*> constSceneTerrains;
        constSceneTerrains.insert(constSceneTerrains.begin(), sceneTerrains.begin(), sceneTerrains.end());

        return constSceneTerrains;
    }

    void TerrainController::addSceneTerrain(SceneTerrain* sceneTerrain) {
        getMapHandler()->getMap()->addSceneTerrain(sceneTerrain);

        markModified();
    }

    void TerrainController::removeSceneTerrain(const SceneTerrain* constSceneTerrain) {
        SceneTerrain* sceneTerrain = findSceneTerrain(constSceneTerrain);
        getMapHandler()->getMap()->removeSceneTerrain(sceneTerrain);

        markModified();
    }

    const SceneTerrain* TerrainController::updateSceneTerrainGeneralProperties(const SceneTerrain* constSceneTerrain,
                                                                               const Point3<float>& position, float ambient) {
        SceneTerrain* sceneTerrain = findSceneTerrain(constSceneTerrain);
        Terrain* terrain = sceneTerrain->getTerrain();

        terrain->setPosition(position);
        if (sceneTerrain->getRigidBody()) {
            sceneTerrain->getRigidBody()->setTransform(PhysicsTransform(position));
        }

        terrain->setAmbient(ambient);

        markModified();
        return sceneTerrain;
    }

    const SceneTerrain* TerrainController::updateSceneTerrainMesh(const SceneTerrain* constSceneTerrain, float xzScale, float yScale) {
        SceneTerrain* sceneTerrain = findSceneTerrain(constSceneTerrain);
        Terrain* terrain = sceneTerrain->getTerrain();

        auto terrainMesh = std::make_shared<TerrainMesh>(terrain->getMesh()->getHeightFilename(), xzScale, yScale);
        terrain->setMesh(terrainMesh);

        markModified();
        return sceneTerrain;
    }

    const SceneTerrain* TerrainController::updateSceneTerrainMaterial(const SceneTerrain* constSceneTerrain, float sRepeat, float tRepeat, const std::string& maskMapFilename,
                                                                      const std::vector<std::string>& materialFilenames) {
        SceneTerrain* sceneTerrain = findSceneTerrain(constSceneTerrain);
        Terrain* terrain = sceneTerrain->getTerrain();

        auto terrainMaterial = std::make_unique<TerrainMaterial>(maskMapFilename, materialFilenames, sRepeat, tRepeat);
        terrain->setMaterial(terrainMaterial);

        markModified();
        return sceneTerrain;
    }

    const SceneTerrain* TerrainController::updateSceneTerrainGrass(const SceneTerrain* constSceneTerrain, const std::string& grassTextureFilename, const std::string& grassMaskFilename,
                                                                   unsigned int numGrassInTex, float grassQuantity, float grassHeight, float grassLength,
                                                                   const Vector3<float>& windDirection, float windStrength) {
        SceneTerrain* sceneTerrain = findSceneTerrain(constSceneTerrain);
        Terrain* terrain = sceneTerrain->getTerrain();
        TerrainGrass* terrainGrass = terrain->getGrass();

        terrainGrass->setGrassTexture(grassTextureFilename);
        terrainGrass->setMaskTexture(grassMaskFilename);
        terrainGrass->setNumGrassInTexture(numGrassInTex);
        if (terrainGrass->getGrassQuantity() != grassQuantity) {
            terrainGrass->setGrassQuantity(grassQuantity);
        }
        terrainGrass->setGrassHeight(grassHeight);
        terrainGrass->setGrassLength(grassLength);
        terrainGrass->setWindDirection(windDirection);
        terrainGrass->setWindStrength(windStrength);

        markModified();
        return sceneTerrain;
    }

    SceneTerrain* TerrainController::findSceneTerrain(const SceneTerrain* constSceneTerrain) {
        const std::list<SceneTerrain*>& sceneTerrains = getMapHandler()->getMap()->getSceneTerrains();
        auto it = std::find(sceneTerrains.begin(), sceneTerrains.end(), constSceneTerrain);

        if (it != sceneTerrains.end()) {
            return *it;
        }

        throw std::invalid_argument("Impossible to find scene terrain: " + constSceneTerrain->getName());
    }
}
