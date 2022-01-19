#include <stdexcept>
#include <memory>

#include <map/Map.h>

namespace urchin {

    Map::Map(Renderer3d& renderer3d, PhysicsWorld& physicsWorld, SoundEnvironment& soundEnvironment, AIEnvironment& aiEnvironment) :
            renderer3d(renderer3d),
            physicsWorld(physicsWorld),
            soundEnvironment(soundEnvironment),
            aiEnvironment(aiEnvironment) {

    }

    Renderer3d& Map::getRenderer3d() const {
        return renderer3d;
    }

    PhysicsWorld& Map::getPhysicsWorld() const {
        return physicsWorld;
    }

    SoundEnvironment& Map::getSoundEnvironment() const {
        return soundEnvironment;
    }

    AIEnvironment& Map::getAIEnvironment() const {
        return aiEnvironment;
    }

    void Map::setRelativeWorkingDirectory(const std::string& relativeWorkingDirectory) {
        this->relativeWorkingDirectory = relativeWorkingDirectory;
    }

    const std::string& Map::getRelativeWorkingDirectory() const {
        return relativeWorkingDirectory;
    }

    const std::list<std::unique_ptr<ObjectEntity>>& Map::getObjectEntities() const {
        return objectEntities;
    }

    ObjectEntity& Map::getObjectEntity(const std::string& name) const {
        for (auto& objectEntity : objectEntities) {
            if (objectEntity->getName() == name) {
                return *objectEntity;
            }
        }
        throw std::invalid_argument("Impossible to find a object entity having name: " + name);
    }

    void Map::findObjectEntitiesByTag(const std::string& tag, std::vector<ObjectEntity*>& models) const {
        objectEntitiesTagHolder.findByTag<ObjectEntity*>(tag, models);
    }

    ObjectEntity* Map::findObjectEntityByTag(const std::string& tag) const {
        tmpObjectEntities.clear();
        objectEntitiesTagHolder.findByTag<ObjectEntity*>(tag, tmpObjectEntities);
        if (tmpObjectEntities.size() > 1) {
            throw std::runtime_error("Impossible to find an unique object entity (" + std::to_string(tmpObjectEntities.size()) + " found) with tag: " + tag);
        }
        if (tmpObjectEntities.empty()) {
            return nullptr;
        }
        return tmpObjectEntities[0];
    }

    ObjectEntity& Map::addObjectEntity(std::unique_ptr<ObjectEntity> objectEntity) {
        objectEntity->setup(renderer3d, physicsWorld, aiEnvironment);
        objectEntitiesTagHolder.addTaggableResource(*objectEntity);
        objectEntities.push_back(std::move(objectEntity));
        return *objectEntities.back();
    }

    void Map::removeObjectEntity(ObjectEntity& objectEntity) {
        objectEntitiesTagHolder.removeTaggableResource(objectEntity);
        objectEntities.remove_if([&objectEntity](const auto& o){ return o.get()==&objectEntity; });
    }

    const std::list<std::unique_ptr<LightEntity>>& Map::getLightEntities() const {
        return lightEntities;
    }

    LightEntity& Map::getLightEntity(const std::string& name) const {
        for (auto& lightEntity : lightEntities) {
            if (lightEntity->getName() == name) {
                return *lightEntity;
            }
        }
        throw std::invalid_argument("Impossible to find a light entity having name: " + name);
    }

    LightEntity& Map::addLightEntity(std::unique_ptr<LightEntity> lightEntity) {
        lightEntity->setup(renderer3d.getLightManager());
        lightEntities.push_back(std::move(lightEntity));
        return *lightEntities.back();
    }

    void Map::removeLightEntity(LightEntity& lightEntity) {
        lightEntities.remove_if([&lightEntity](const auto& o){return o.get()==&lightEntity;});
    }

    const std::list<std::unique_ptr<TerrainEntity>>& Map::getTerrainEntities() const {
        return terrainEntities;
    }

    TerrainEntity& Map::getTerrainEntity(const std::string& name) const {
        for (auto& terrainEntity : terrainEntities) {
            if (terrainEntity->getName() == name) {
                return *terrainEntity;
            }
        }
        throw std::invalid_argument("Impossible to find a terrain entity having name: " + name);
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

    WaterEntity& Map::getWaterEntity(const std::string& name) const {
        for (auto& waterEntity : waterEntities) {
            if (waterEntity->getName() == name) {
                return *waterEntity;
            }
        }
        throw std::invalid_argument("Impossible to find a water entity having name: " + name);
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

    const std::list<std::unique_ptr<SoundEntity>>& Map::getSoundEntities() const {
        return soundEntities;
    }

    SoundEntity& Map::getSoundEntity(const std::string& name) const {
        for (auto& soundEntity : soundEntities) {
            if (soundEntity->getName() == name) {
                return *soundEntity;
            }
        }
        throw std::invalid_argument("Impossible to find a sound entity having name: " + name);
    }

    SoundEntity& Map::addSoundEntity(std::unique_ptr<SoundEntity> soundEntity) {
        soundEntity->setup(soundEnvironment);
        soundEntities.push_back(std::move(soundEntity));
        return *soundEntities.back();
    }

    void Map::removeSoundEntity(SoundEntity& soundEntity) {
        soundEntities.remove_if([&soundEntity](const auto& o){return o.get()==&soundEntity;});
    }

    void Map::pause() {
        renderer3d.pause();
        physicsWorld.pause();
        aiEnvironment.pause();
        soundEnvironment.pause();
    }

    void Map::unpause() {
        renderer3d.unpause();
        physicsWorld.unpause();
        aiEnvironment.unpause();
        soundEnvironment.unpause();
    }

    void Map::refresh() {
        physicsWorld.checkNoExceptionRaised();
        aiEnvironment.checkNoExceptionRaised();
        soundEnvironment.checkNoExceptionRaised();

        for (const auto& objectEntity : objectEntities) {
            objectEntity->refresh();
        }

        for (const auto& terrainEntity : terrainEntities) {
            terrainEntity->refresh();
        }

        if (renderer3d.getCamera()) {
            soundEnvironment.process(renderer3d.getCamera()->getPosition(), renderer3d.getCamera()->getView(), renderer3d.getCamera()->getUp());
        } else {
            soundEnvironment.process();
        }
    }
}

