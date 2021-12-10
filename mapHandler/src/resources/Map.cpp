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

    void Map::loadFrom(const UdaChunk* sceneChunk, const UdaParser& udaParser, LoadMapCallback& loadMapCallback) {
        if (renderer3d && !renderer3d->isPaused()) { //to avoid move camera before being able to see the map
            throw std::runtime_error("Renderer 3d should be paused while loading map.");
        }

        if (physicsWorld && !physicsWorld->isPaused()) { //to avoid miss of collision between objects just loaded and on objects not loaded yet
            throw std::runtime_error("Physics world should be paused while loading map.");
        }

        if (aiEnvironment && !aiEnvironment->isPaused()) { //to avoid compute path based on a world with missing objects
            throw std::runtime_error("AI environment should be paused while loading map.");
        }

        loadMapCallback.notify(LoadMapCallback::MODELS, LoadMapCallback::START_LOADING);
        loadObjectEntities(sceneChunk, udaParser);
        loadMapCallback.notify(LoadMapCallback::MODELS, LoadMapCallback::LOADED);

        loadMapCallback.notify(LoadMapCallback::LIGHTS, LoadMapCallback::START_LOADING);
        loadLightEntities(sceneChunk, udaParser);
        loadMapCallback.notify(LoadMapCallback::LIGHTS, LoadMapCallback::LOADED);

        loadMapCallback.notify(LoadMapCallback::LANDSCAPE, LoadMapCallback::START_LOADING);
        loadTerrainEntities(sceneChunk, udaParser);
        loadWaterEntities(sceneChunk, udaParser);
        loadSkyEntity(sceneChunk, udaParser);
        loadMapCallback.notify(LoadMapCallback::LANDSCAPE, LoadMapCallback::LOADED);

        loadMapCallback.notify(LoadMapCallback::SOUNDS, LoadMapCallback::START_LOADING);
        loadSoundEntities(sceneChunk, udaParser);
        loadMapCallback.notify(LoadMapCallback::SOUNDS, LoadMapCallback::LOADED);

        loadMapCallback.notify(LoadMapCallback::AI, LoadMapCallback::START_LOADING);
        loadAIConfig(sceneChunk, udaParser);
        loadMapCallback.notify(LoadMapCallback::AI, LoadMapCallback::LOADED);
    }

    void Map::loadObjectEntities(const UdaChunk* sceneChunk, const UdaParser& udaParser) {
        auto modelsListChunk = udaParser.getUniqueChunk(true, MODELS_TAG, UdaAttribute(), sceneChunk);
        auto modelsChunk = udaParser.getChunks(MODEL_TAG, UdaAttribute(), modelsListChunk);

        for (const auto& modelChunk : modelsChunk) {
            auto sceneModel = std::make_unique<SceneModel>();
            sceneModel->loadFrom(modelChunk, udaParser);

            addSceneModel(std::move(sceneModel));
        }
        renderer3d->preWarmModels();
    }

    void Map::loadLightEntities(const UdaChunk* sceneChunk, const UdaParser& udaParser) {
        auto lightsListChunk = udaParser.getUniqueChunk(true, LIGHTS_TAG, UdaAttribute(), sceneChunk);
        auto lightsChunk = udaParser.getChunks(LIGHT_TAG, UdaAttribute(), lightsListChunk);

        for (const auto& lightChunk : lightsChunk) {
            auto lightEntity = std::make_unique<LightEntity>();
            lightEntity->loadFrom(lightChunk, udaParser);

            addLightEntity(std::move(lightEntity));
        }
    }

    void Map::loadTerrainEntities(const UdaChunk* sceneChunk, const UdaParser& udaParser) {
        auto terrainsListChunk = udaParser.getUniqueChunk(true, TERRAINS_TAG, UdaAttribute(), sceneChunk);
        auto terrainsChunk = udaParser.getChunks(TERRAIN_TAG, UdaAttribute(), terrainsListChunk);

        for (const auto& terrainChunk : terrainsChunk) {
            auto terrainEntity = std::make_unique<TerrainEntity>();
            terrainEntity->loadFrom(terrainChunk, udaParser);

            addTerrainEntity(std::move(terrainEntity));
        }
    }

    void Map::loadWaterEntities(const UdaChunk* sceneChunk, const UdaParser& udaParser) {
        auto watersListChunk = udaParser.getUniqueChunk(true, WATERS_TAG, UdaAttribute(), sceneChunk);
        auto watersChunk = udaParser.getChunks(WATER_TAG, UdaAttribute(), watersListChunk);

        for (const auto& waterChunk : watersChunk) {
            auto waterEntity = std::make_unique<WaterEntity>();
            waterEntity->loadFrom(waterChunk, udaParser);

            addWaterEntity(std::move(waterEntity));
        }
    }

    void Map::loadSkyEntity(const UdaChunk* sceneChunk, const UdaParser& udaParser) {
        auto skyChunk = udaParser.getUniqueChunk(true, SKY_TAG, UdaAttribute(), sceneChunk);

        skyEntity.loadFrom(skyChunk, udaParser);
    }

    void Map::loadSoundEntities(const UdaChunk* sceneChunk, const UdaParser& udaParser) {
        auto soundElementsListChunk = udaParser.getUniqueChunk(true, SOUND_ELEMENTS_TAG, UdaAttribute(), sceneChunk);
        auto soundElementsChunk = udaParser.getChunks(SOUND_ELEMENT_TAG, UdaAttribute(), soundElementsListChunk);

        for (const auto& soundElementChunk : soundElementsChunk) {
            auto soundEntity = std::make_unique<SoundEntity>();
            soundEntity->loadFrom(soundElementChunk, udaParser);

            addSoundEntity(std::move(soundEntity));
        }
    }

    void Map::loadAIConfig(const UdaChunk* sceneChunk, const UdaParser& udaParser) {
        auto aiElementsListChunk = udaParser.getUniqueChunk(true, AI_ELEMENTS_TAG, UdaAttribute(), sceneChunk);
        aiEnvironment->getNavMeshGenerator().setNavMeshAgent(NavMeshAgentReaderWriter::loadFrom(aiElementsListChunk, udaParser));
    }

    void Map::writeOn(UdaChunk& sceneChunk, UdaWriter& udaWriter) const {
        writeObjectEntities(sceneChunk, udaWriter);
        writeLightEntities(sceneChunk, udaWriter);
        writeTerrainEntities(sceneChunk, udaWriter);
        writeWaterEntities(sceneChunk, udaWriter);
        writeSkyEntity(sceneChunk, udaWriter);
        writeSoundEntities(sceneChunk, udaWriter);
        writeAIConfig(sceneChunk, udaWriter);
    }

    void Map::writeObjectEntities(UdaChunk& sceneChunk, UdaWriter& udaWriter) const {
        auto& modelsListChunk = udaWriter.createChunk(MODELS_TAG, UdaAttribute(), &sceneChunk);

        for (auto& sceneModel : sceneModels) {
            auto& modelsChunk = udaWriter.createChunk(MODEL_TAG, UdaAttribute(), &modelsListChunk);
            sceneModel->writeOn(modelsChunk, udaWriter);
        }
    }

    void Map::writeLightEntities(UdaChunk& sceneChunk, UdaWriter& udaWriter) const {
        auto& lightsListChunk = udaWriter.createChunk(LIGHTS_TAG, UdaAttribute(), &sceneChunk);

        for (auto& lightEntity : lightEntities) {
            auto& lightsChunk = udaWriter.createChunk(LIGHT_TAG, UdaAttribute(), &lightsListChunk);
            lightEntity->writeOn(lightsChunk, udaWriter);
        }
    }

    void Map::writeTerrainEntities(UdaChunk& sceneChunk, UdaWriter& udaWriter) const {
        auto& terrainsListChunk = udaWriter.createChunk(TERRAINS_TAG, UdaAttribute(), &sceneChunk);

        for (auto& terrainEntity : terrainEntities) {
            auto& terrainsChunk = udaWriter.createChunk(TERRAIN_TAG, UdaAttribute(), &terrainsListChunk);
            terrainEntity->writeOn(terrainsChunk, udaWriter);
        }
    }

    void Map::writeWaterEntities(UdaChunk& sceneChunk, UdaWriter& udaWriter) const {
        auto& watersListChunk = udaWriter.createChunk(WATERS_TAG, UdaAttribute(), &sceneChunk);

        for (auto& waterEntity : waterEntities) {
            auto& watersChunk = udaWriter.createChunk(WATER_TAG, UdaAttribute(), &watersListChunk);
            waterEntity->writeOn(watersChunk, udaWriter);
        }
    }

    void Map::writeSkyEntity(UdaChunk& sceneChunk, UdaWriter& udaWriter) const {
        auto& skyChunk = udaWriter.createChunk(SKY_TAG, UdaAttribute(), &sceneChunk);

        skyEntity.writeOn(skyChunk, udaWriter);
    }

    void Map::writeSoundEntities(UdaChunk& sceneChunk, UdaWriter& udaWriter) const {
        auto& soundElementsListChunk = udaWriter.createChunk(SOUND_ELEMENTS_TAG, UdaAttribute(), &sceneChunk);

        for (auto& soundEntity : soundEntities) {
            auto& soundElementsChunk = udaWriter.createChunk(SOUND_ELEMENT_TAG, UdaAttribute(), &soundElementsListChunk);
            soundEntity->writeOn(soundElementsChunk, udaWriter);
        }
    }

    void Map::writeAIConfig(UdaChunk& sceneChunk, UdaWriter& udaWriter) const {
        auto& aiElementsListChunk = udaWriter.createChunk(AI_ELEMENTS_TAG, UdaAttribute(), &sceneChunk);
        NavMeshAgentReaderWriter::writeOn(aiElementsListChunk, aiEnvironment->getNavMeshGenerator().getNavMeshAgent(), udaWriter);
    }

    const std::list<std::unique_ptr<SceneModel>>& Map::getSceneModels() const {
        return sceneModels;
    }

    SceneModel& Map::getSceneModel(const std::string& name) const {
        for (auto& sceneModel : sceneModels) {
            if (sceneModel->getName() == name) {
                return *sceneModel;
            }
        }
        throw std::invalid_argument("Impossible to find a object entity having name: " + name);
    }

    void Map::findSceneModelsByTag(const std::string& tag, std::vector<SceneModel*>& models) const {
        modelTagHolder.findByTag<SceneModel*>(tag, models);
    }

    void Map::addSceneModel(std::unique_ptr<SceneModel> sceneModel) {
        sceneModel->setup(renderer3d, physicsWorld, aiEnvironment);
        modelTagHolder.addTaggableResource(*sceneModel);
        sceneModels.push_back(std::move(sceneModel));
    }

    void Map::removeSceneModel(SceneModel& sceneModel) {
        modelTagHolder.removeTaggableResource(sceneModel);
        sceneModels.remove_if([&sceneModel](const auto& o){return o.get()==&sceneModel;});
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

        for (const auto& sceneModel : sceneModels) {
            sceneModel->refresh();
        }
        for (const auto& terrainEntity : terrainEntities) {
            terrainEntity->refresh();
        }
        refreshSound();
    }

    void Map::refreshSound() {
        //update sound event
        if (renderer3d && renderer3d->getCamera()) {
            soundEnvironment->process(renderer3d->getCamera()->getPosition(), renderer3d->getCamera()->getView(), renderer3d->getCamera()->getUp());
        } else {
            soundEnvironment->process();
        }
    }
}

