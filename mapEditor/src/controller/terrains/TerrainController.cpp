#include <stdexcept>

#include <controller/terrains/TerrainController.h>

namespace urchin {
    TerrainController::TerrainController() :
            AbstractController() {

    }

    std::list<const SceneTerrain*> TerrainController::getSceneTerrains() const {
        const auto& sceneTerrains = getMapHandler()->getMap().getSceneTerrains();
        std::list<const SceneTerrain*> constSceneTerrains;
        for(auto& sceneTerrain : sceneTerrains) {
            constSceneTerrains.emplace_back(sceneTerrain.get());
        }

        return constSceneTerrains;
    }

    void TerrainController::addSceneTerrain(std::unique_ptr<SceneTerrain> sceneTerrain) {
        getMapHandler()->getMap().addSceneTerrain(std::move(sceneTerrain));

        markModified();
    }

    void TerrainController::removeSceneTerrain(const SceneTerrain& constSceneTerrain) {
        SceneTerrain& sceneTerrain = findSceneTerrain(constSceneTerrain);
        getMapHandler()->getMap().removeSceneTerrain(sceneTerrain);

        markModified();
    }

    const SceneTerrain& TerrainController::updateSceneTerrainGeneralProperties(const SceneTerrain& constSceneTerrain,
                                                                               const Point3<float>& position, float ambient) {
        const SceneTerrain& sceneTerrain = findSceneTerrain(constSceneTerrain);
        Terrain* terrain = sceneTerrain.getTerrain();

        terrain->setPosition(position);
        if (sceneTerrain.getRigidBody()) {
            sceneTerrain.getRigidBody()->setTransform(PhysicsTransform(position));
        }

        terrain->setAmbient(ambient);

        markModified();
        return sceneTerrain;
    }

    const SceneTerrain& TerrainController::updateSceneTerrainMesh(const SceneTerrain& constSceneTerrain, float xzScale, float yScale) {
        const SceneTerrain& sceneTerrain = findSceneTerrain(constSceneTerrain);
        Terrain* terrain = sceneTerrain.getTerrain();

        auto terrainMesh = std::make_unique<TerrainMesh>(terrain->getMesh()->getHeightFilename(), xzScale, yScale);
        terrain->setMesh(std::move(terrainMesh));

        markModified();
        return sceneTerrain;
    }

    const SceneTerrain& TerrainController::updateSceneTerrainMaterial(const SceneTerrain& constSceneTerrain, float sRepeat, float tRepeat, const std::string& maskMapFilename,
                                                                      const std::vector<std::string>& materialFilenames) {
        const SceneTerrain& sceneTerrain = findSceneTerrain(constSceneTerrain);
        Terrain* terrain = sceneTerrain.getTerrain();

        auto terrainMaterials = std::make_unique<TerrainMaterials>(maskMapFilename, materialFilenames, sRepeat, tRepeat);
        terrain->setMaterials(std::move(terrainMaterials));

        markModified();
        return sceneTerrain;
    }

    const SceneTerrain& TerrainController::updateSceneTerrainGrass(const SceneTerrain& constSceneTerrain, const std::string& grassTextureFilename, const std::string& grassMaskFilename,
                                                                   unsigned int numGrassInTex, float grassQuantity, float grassHeight, float grassLength,
                                                                   const Vector3<float>& windDirection, float windStrength) {
        const SceneTerrain& sceneTerrain = findSceneTerrain(constSceneTerrain);
        Terrain* terrain = sceneTerrain.getTerrain();
        TerrainGrass& terrainGrass = terrain->getGrass();

        terrainGrass.setGrassTexture(grassTextureFilename);
        terrainGrass.setMaskTexture(grassMaskFilename);
        terrainGrass.setNumGrassInTexture(numGrassInTex);
        if (terrainGrass.getGrassQuantity() != grassQuantity) {
            terrainGrass.setGrassQuantity(grassQuantity);
        }
        terrainGrass.setGrassHeight(grassHeight);
        terrainGrass.setGrassLength(grassLength);
        terrainGrass.setWindDirection(windDirection);
        terrainGrass.setWindStrength(windStrength);

        markModified();
        return sceneTerrain;
    }

    SceneTerrain& TerrainController::findSceneTerrain(const SceneTerrain& constSceneTerrain) {
        const auto& sceneTerrains = getMapHandler()->getMap().getSceneTerrains();
        auto it = std::ranges::find_if(sceneTerrains, [&constSceneTerrain](const auto& o){return o.get() == &constSceneTerrain;});

        if (it != sceneTerrains.end()) {
            return *(*it);
        }

        throw std::invalid_argument("Impossible to find scene terrain: " + constSceneTerrain.getName());
    }
}
