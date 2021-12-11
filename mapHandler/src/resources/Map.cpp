#include <stdexcept>
#include <memory>

#include <resources/Map.h>
#include <resources/ai/NavMeshAgentReaderWriter.h>

namespace urchin {

    Map::Map(Renderer3d* renderer3d, PhysicsWorld* physicsWorld, SoundEnvironment* soundEnvironment, AIEnvironment* aiEnvironment) :
            renderer3d(renderer3d),
            physicsWorld(physicsWorld),
            soundEnvironment(soundEnvironment),
            aiEnvironment(aiEnvironment),
            skyEntity(SkyEntity(renderer3d)) {
        if (!renderer3d) {
            throw std::invalid_argument("Renderer 3d cannot be null in map");
        }
        if (!soundEnvironment) {
            throw std::invalid_argument("Sound environment cannot be null in map");
        }
        if (!aiEnvironment) {
            throw std::invalid_argument("AI environment cannot be null in map");
        }
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

    void Map::addObjectEntity(std::unique_ptr<ObjectEntity> objectEntity) {
        objectEntity->setup(renderer3d, physicsWorld, aiEnvironment);
        objectEntitiesTagHolder.addTaggableResource(*objectEntity);
        objectEntities.push_back(std::move(objectEntity));
    }

    void Map::removeObjectEntity(ObjectEntity& objectEntity) {
        objectEntitiesTagHolder.removeTaggableResource(objectEntity);
        objectEntities.remove_if([&objectEntity](const auto& o){return o.get()==&objectEntity;});
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

    void Map::addLightEntity(std::unique_ptr<LightEntity> lightEntity) {
        lightEntity->setup(renderer3d->getLightManager());
        lightEntities.push_back(std::move(lightEntity));
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

    void Map::addTerrainEntity(std::unique_ptr<TerrainEntity> terrainEntity) {
        terrainEntity->setup(renderer3d, physicsWorld, aiEnvironment);
        terrainEntities.push_back(std::move(terrainEntity));
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

    void Map::addWaterEntity(std::unique_ptr<WaterEntity> waterEntity) {
        waterEntity->setup(renderer3d);
        waterEntities.push_back(std::move(waterEntity));
    }

    void Map::removeWaterEntity(WaterEntity& waterEntity) {
        waterEntities.remove_if([&waterEntity](const auto& o){return o.get()==&waterEntity;});
    }

    const SkyEntity& Map::getSkyEntity() const {
        return skyEntity;
    }

    void Map::updateSkyEntity(std::unique_ptr<Skybox> skybox) {
        skyEntity.changeSkybox(std::move(skybox));
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

    void Map::addSoundEntity(std::unique_ptr<SoundEntity> soundEntity) {
        soundEntity->setup(soundEnvironment);
        soundEntities.push_back(std::move(soundEntity));
    }

    void Map::removeSoundEntity(SoundEntity& soundEntity) {
        soundEntities.remove_if([&soundEntity](const auto& o){return o.get()==&soundEntity;});
    }

    void Map::pause() {
        if (renderer3d) {
            renderer3d->pause();
        }

        if (physicsWorld) {
            physicsWorld->pause();
        }

        if (aiEnvironment) {
            aiEnvironment->pause();
        }

        if (soundEnvironment) {
            soundEnvironment->pause();
        }
    }

    void Map::unpause() {
        if (renderer3d) {
            renderer3d->unpause();
        }

        if (physicsWorld) {
            physicsWorld->unpause();
        }

        if (aiEnvironment) {
            aiEnvironment->unpause();
        }

        if (soundEnvironment) {
            soundEnvironment->unpause();
        }
    }

    void Map::refresh() {
        physicsWorld->checkNoExceptionRaised();
        aiEnvironment->checkNoExceptionRaised();
        soundEnvironment->checkNoExceptionRaised();

        for (const auto& objectEntity : objectEntities) {
            objectEntity->refresh();
        }

        for (const auto& terrainEntity : terrainEntities) {
            terrainEntity->refresh();
        }

        if (renderer3d && renderer3d->getCamera()) {
            soundEnvironment->process(renderer3d->getCamera()->getPosition(), renderer3d->getCamera()->getView(), renderer3d->getCamera()->getUp());
        } else {
            soundEnvironment->process();
        }
    }
}

