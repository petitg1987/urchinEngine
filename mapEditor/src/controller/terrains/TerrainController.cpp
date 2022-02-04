#include <stdexcept>

#include <controller/terrains/TerrainController.h>

namespace urchin {
    TerrainController::TerrainController() :
            AbstractController() {

    }

    std::list<const TerrainEntity*> TerrainController::getTerrainEntities() const {
        const auto& terrainEntities = getMap().getTerrainEntities();
        std::list<const TerrainEntity*> constTerrainEntities;
        for (auto& terrainEntity : terrainEntities) {
            constTerrainEntities.emplace_back(terrainEntity.get());
        }

        return constTerrainEntities;
    }

    void TerrainController::addTerrainEntity(std::unique_ptr<TerrainEntity> terrainEntity) {
        getMap().addTerrainEntity(std::move(terrainEntity));

        markModified();
    }

    void TerrainController::removeTerrainEntity(const TerrainEntity& constTerrainEntity) {
        TerrainEntity& terrainEntity = findTerrainEntity(constTerrainEntity);
        getMap().removeTerrainEntity(terrainEntity);

        markModified();
    }

    const TerrainEntity& TerrainController::updateTerrainGeneralProperties(const TerrainEntity& constTerrainEntity,
                                                                           const Point3<float>& position, float ambient) {
        const TerrainEntity& terrainEntity = findTerrainEntity(constTerrainEntity);
        Terrain* terrain = terrainEntity.getTerrain();

        terrain->setPosition(position);
        if (terrainEntity.getRigidBody()) {
            terrainEntity.getRigidBody()->setTransform(PhysicsTransform(position));
        }

        terrain->setAmbient(ambient);

        markModified();
        return terrainEntity;
    }

    const TerrainEntity& TerrainController::updateTerrainMesh(const TerrainEntity& constTerrainEntity, float xzScale, float yScale) {
        const TerrainEntity& terrainEntity = findTerrainEntity(constTerrainEntity);
        Terrain* terrain = terrainEntity.getTerrain();

        auto terrainMesh = std::make_unique<TerrainMesh>(terrain->getMesh()->getHeightFilename(), xzScale, yScale);
        terrain->setMesh(std::move(terrainMesh));

        markModified();
        return terrainEntity;
    }

    const TerrainEntity& TerrainController::updateTerrainMaterial(const TerrainEntity& constTerrainEntity, float sRepeat, float tRepeat, std::string maskMapFilename,
                                                                  const std::vector<std::string>& materialFilenames) {
        const TerrainEntity& terrainEntity = findTerrainEntity(constTerrainEntity);
        Terrain* terrain = terrainEntity.getTerrain();

        auto terrainMaterials = std::make_unique<TerrainMaterials>(std::move(maskMapFilename), materialFilenames, sRepeat, tRepeat);
        terrain->setMaterials(std::move(terrainMaterials));

        markModified();
        return terrainEntity;
    }

    const TerrainEntity& TerrainController::updateTerrainGrass(const TerrainEntity& constTerrainEntity, std::string grassTextureFilename, std::string grassMaskFilename,
                                                               unsigned int numGrassInTex, float grassQuantity, float grassHeight, float grassLength,
                                                               const Vector3<float>& windDirection, float windStrength) {
        const TerrainEntity& terrainEntity = findTerrainEntity(constTerrainEntity);
        Terrain* terrain = terrainEntity.getTerrain();
        TerrainGrass& terrainGrass = terrain->getGrass();

        terrainGrass.setGrassTexture(std::move(grassTextureFilename));
        terrainGrass.setMaskTexture(std::move(grassMaskFilename));
        terrainGrass.setNumGrassInTexture(numGrassInTex);
        if (terrainGrass.getGrassQuantity() != grassQuantity) {
            terrainGrass.setGrassQuantity(grassQuantity);
        }
        terrainGrass.setGrassHeight(grassHeight);
        terrainGrass.setGrassLength(grassLength);
        terrainGrass.setWindDirection(windDirection);
        terrainGrass.setWindStrength(windStrength);

        markModified();
        return terrainEntity;
    }

    TerrainEntity& TerrainController::findTerrainEntity(const TerrainEntity& constTerrainEntity) {
        const auto& terrainsEntities = getMap().getTerrainEntities();
        auto it = std::ranges::find_if(terrainsEntities, [&constTerrainEntity](const auto& o){return o.get() == &constTerrainEntity;});

        if (it != terrainsEntities.end()) {
            return *(*it);
        }

        throw std::invalid_argument("Impossible to find terrain entity: " + constTerrainEntity.getName());
    }
}
