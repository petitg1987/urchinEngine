#include <stdexcept>
#include <memory>

#include "map/Map.h"

namespace urchin {

    Map::Map(Renderer3d* renderer3d, PhysicsWorld* physicsWorld, SoundEnvironment* soundEnvironment, AIEnvironment* aiEnvironment) :
            renderer3d(renderer3d),
            physicsWorld(physicsWorld),
            soundEnvironment(soundEnvironment),
            aiEnvironment(aiEnvironment),
            skyEntity(std::make_unique<SkyEntity>()),
            navMeshAgentEntity(std::make_unique<NavMeshAgentEntity>()) {

    }

    Renderer3d* Map::getRenderer3d() const {
        return renderer3d;
    }

    PhysicsWorld* Map::getPhysicsWorld() const {
        return physicsWorld;
    }

    SoundEnvironment* Map::getSoundEnvironment() const {
        return soundEnvironment;
    }

    AIEnvironment* Map::getAIEnvironment() const {
        return aiEnvironment;
    }

    void Map::setRelativeWorkingDirectory(std::string relativeWorkingDirectory) {
        this->relativeWorkingDirectory = std::move(relativeWorkingDirectory);
    }

    const std::string& Map::getRelativeWorkingDirectory() const {
        return relativeWorkingDirectory;
    }

    void Map::setLightMaskNames(const std::array<std::string, 8>& lightMaskNames) {
        this->lightMaskNames = lightMaskNames;
    }

    const std::array<std::string, 8>& Map::getLightMaskNames() const {
        return lightMaskNames;
    }

    const std::list<std::unique_ptr<ObjectEntity>>& Map::getObjectEntities() const {
        return objectEntities;
    }

    ObjectEntity& Map::getObjectEntity(std::string_view name, const std::vector<std::string>& groupHierarchy) const {
        for (auto& objectEntity : objectEntities) {
            if (objectEntity->getName() == name && objectEntity->getGroupHierarchy() == groupHierarchy) {
                return *objectEntity;
            }
        }
        throw std::invalid_argument("Impossible to find a object entity having name: " + std::string(name));
    }

    void Map::findObjectEntitiesByTag(std::string_view tag, std::vector<ObjectEntity*>& objects) const {
        objectEntitiesTagHolder.findByTag<ObjectEntity*>(tag, objects);
    }

    std::vector<ObjectEntity*> Map::findObjectEntitiesByTag(std::string_view tag) const {
        std::vector<ObjectEntity*> objects;
        objectEntitiesTagHolder.findByTag<ObjectEntity*>(tag, objects);
        return objects;
    }

    ObjectEntity* Map::findObjectEntityByTag(std::string_view tag) const {
        tmpObjectEntities.clear();
        objectEntitiesTagHolder.findByTag<ObjectEntity*>(tag, tmpObjectEntities);
        if (tmpObjectEntities.size() > 1) {
            throw std::runtime_error("Impossible to find an unique object entity (" + std::to_string(tmpObjectEntities.size()) + " found) with tag: " + std::string(tag));
        }
        if (tmpObjectEntities.empty()) {
            return nullptr;
        }
        return tmpObjectEntities[0];
    }

    ObjectEntity& Map::addObjectEntity(std::unique_ptr<ObjectEntity> objectEntity) {
        objectEntity->setup(renderer3d, physicsWorld, soundEnvironment, aiEnvironment);
        objectEntitiesTagHolder.addTaggableResource(*objectEntity);

        objectEntities.push_back(std::move(objectEntity));
        return *objectEntities.back();
    }

    void Map::removeObjectEntity(ObjectEntity& objectEntity) {
        objectEntitiesTagHolder.removeTaggableResource(objectEntity);
        objectEntities.remove_if([&objectEntity](const auto& o){ return o.get() == &objectEntity; });
    }

    const std::list<std::unique_ptr<TerrainEntity>>& Map::getTerrainEntities() const {
        return terrainEntities;
    }

    TerrainEntity& Map::getTerrainEntity(std::string_view name) const {
        for (auto& terrainEntity : terrainEntities) {
            if (terrainEntity->getName() == name) {
                return *terrainEntity;
            }
        }
        throw std::invalid_argument("Impossible to find a terrain entity having name: " + std::string(name));
    }

    TerrainEntity& Map::addTerrainEntity(std::unique_ptr<TerrainEntity> terrainEntity) {
        terrainEntity->setup(renderer3d, physicsWorld, aiEnvironment);
        terrainEntities.push_back(std::move(terrainEntity));
        return *terrainEntities.back();
    }

    void Map::removeTerrainEntity(TerrainEntity& terrainEntity) {
        terrainEntities.remove_if([&terrainEntity](const auto& o){return o.get()==&terrainEntity;});
    }

    const std::list<std::unique_ptr<WaterEntity>>& Map::getWaterEntities() const {
        return waterEntities;
    }

    WaterEntity& Map::getWaterEntity(std::string_view name) const {
        for (auto& waterEntity : waterEntities) {
            if (waterEntity->getName() == name) {
                return *waterEntity;
            }
        }
        throw std::invalid_argument("Impossible to find a water entity having name: " + std::string(name));
    }

    WaterEntity& Map::addWaterEntity(std::unique_ptr<WaterEntity> waterEntity) {
        waterEntity->setup(renderer3d);
        waterEntities.push_back(std::move(waterEntity));
        return *waterEntities.back();
    }

    void Map::removeWaterEntity(WaterEntity& waterEntity) {
        waterEntities.remove_if([&waterEntity](const auto& o){return o.get()==&waterEntity;});
    }

    const SkyEntity& Map::getSkyEntity() const {
        return *skyEntity;
    }

    void Map::setSkyEntity(std::unique_ptr<SkyEntity> skyEntity) {
        skyEntity->setup(renderer3d);
        this->skyEntity = std::move(skyEntity);
    }

    const NavMeshAgentEntity& Map::getNavMeshAgentEntity() const {
        return *navMeshAgentEntity;
    }

    void Map::setNavMeshAgentEntity(std::unique_ptr<NavMeshAgentEntity> navMeshAgentEntity) {
        navMeshAgentEntity->setup(aiEnvironment);
        this->navMeshAgentEntity = std::move(navMeshAgentEntity);
    }

    void Map::pause() const {
        if (renderer3d) {
            renderer3d->pause();
        }
        if (physicsWorld) {
            physicsWorld->pause();
        }
        if (soundEnvironment) {
            soundEnvironment->pause();
        }
        if (aiEnvironment) {
            aiEnvironment->pause();
        }
    }

    void Map::unpause() const {
        if (renderer3d) {
            renderer3d->unpause();
        }
        if (physicsWorld) {
            physicsWorld->unpause();
        }
        if (soundEnvironment) {
            soundEnvironment->unpause();
        }
        if (aiEnvironment) {
            aiEnvironment->unpause();
        }
    }

    void Map::refresh() const {
        if (physicsWorld) {
            physicsWorld->checkNoExceptionRaised();
        }
        if (aiEnvironment) {
            aiEnvironment->checkNoExceptionRaised();
        }

        for (const auto& objectEntity : objectEntities) {
            objectEntity->refresh();
        }

        for (const auto& terrainEntity : terrainEntities) {
            terrainEntity->refresh();
        }

        if (soundEnvironment) {
            if (renderer3d) {
                soundEnvironment->process(renderer3d->getCamera().getPosition(), renderer3d->getCamera().getView(), renderer3d->getCamera().getUp());
            } else {
                soundEnvironment->process();
            }
        }
    }
}

