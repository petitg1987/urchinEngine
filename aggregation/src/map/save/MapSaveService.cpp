#include <stdexcept>

#include <map/save/MapSaveService.h>
#include <map/save/ai/NavMeshAgentReaderWriter.h>
#include <map/save/object/ObjectEntityReaderWriter.h>
#include <map/save/terrain/TerrainEntityReaderWriter.h>
#include <map/save/light/LightEntityReaderWriter.h>
#include <map/save/sound/SoundEntityReaderWriter.h>
#include <map/save/water/WaterEntityReaderWriter.h>
#include <map/save/sky/SkyEntityReaderWriter.h>

namespace urchin {

    /**
     * @param map [out] Map to load the entities (must be empty)
     */
    void MapSaveService::loadMap(const std::string& filename, LoadMapCallback& loadMapCallback, const std::unique_ptr<Map>& map) {
        Logger::instance().logInfo("Load map: " + filename);
        UdaParser udaParser(FileSystem::instance().getResourcesDirectory() + filename);

        const UdaChunk* configChunk = udaParser.getUniqueChunk(true, CONFIG_TAG);
        const UdaChunk* workingDirChunk = udaParser.getUniqueChunk(true, WORKING_DIR_TAG, UdaAttribute(), configChunk);
        map->setRelativeWorkingDirectory(workingDirChunk->getStringValue());

        const UdaChunk* sceneChunk = udaParser.getUniqueChunk(true, SCENE_TAG);
        loadMap(*map, sceneChunk, udaParser, loadMapCallback);
    }

    void MapSaveService::loadMap(Map& map, const UdaChunk* sceneChunk, const UdaParser& udaParser, LoadMapCallback& loadMapCallback) {
        if (!map.getRenderer3d().isPaused()) { //to avoid move camera before being able to see the map
            throw std::runtime_error("Renderer 3d should be paused while loading map.");
        }
        if (!map.getPhysicsWorld().isPaused()) { //to avoid miss of collision between objects just loaded and on objects not loaded yet
            throw std::runtime_error("Physics world should be paused while loading map.");
        }
        if (!map.getAIEnvironment().isPaused()) { //to avoid compute path based on a world with missing objects
            throw std::runtime_error("AI environment should be paused while loading map.");
        }

        loadMapCallback.notify(LoadMapCallback::MODELS, LoadMapCallback::START_LOADING);
        loadObjectEntities(map, sceneChunk, udaParser);
        loadMapCallback.notify(LoadMapCallback::MODELS, LoadMapCallback::LOADED);

        loadMapCallback.notify(LoadMapCallback::LIGHTS, LoadMapCallback::START_LOADING);
        loadLightEntities(map, sceneChunk, udaParser);
        loadMapCallback.notify(LoadMapCallback::LIGHTS, LoadMapCallback::LOADED);

        loadMapCallback.notify(LoadMapCallback::LANDSCAPE, LoadMapCallback::START_LOADING);
        loadTerrainEntities(map, sceneChunk, udaParser);
        loadWaterEntities(map, sceneChunk, udaParser);
        loadSkyEntity(map, sceneChunk, udaParser);
        loadMapCallback.notify(LoadMapCallback::LANDSCAPE, LoadMapCallback::LOADED);

        loadMapCallback.notify(LoadMapCallback::SOUNDS, LoadMapCallback::START_LOADING);
        loadSoundEntities(map, sceneChunk, udaParser);
        loadMapCallback.notify(LoadMapCallback::SOUNDS, LoadMapCallback::LOADED);

        loadMapCallback.notify(LoadMapCallback::AI, LoadMapCallback::START_LOADING);
        loadAIConfig(map, sceneChunk, udaParser);
        loadMapCallback.notify(LoadMapCallback::AI, LoadMapCallback::LOADED);
    }

    void MapSaveService::loadObjectEntities(Map& map, const UdaChunk* sceneChunk, const UdaParser& udaParser) {
        auto objectsListChunk = udaParser.getUniqueChunk(true, OBJECTS_TAG, UdaAttribute(), sceneChunk);
        auto objectsChunk = udaParser.getChunks(OBJECT_TAG, UdaAttribute(), objectsListChunk);

        for (const auto& objectChunk : objectsChunk) {
            std::unique_ptr<ObjectEntity> objectEntity = ObjectEntityReaderWriter::load(objectChunk, udaParser);
            map.addObjectEntity(std::move(objectEntity));
        }
        map.getRenderer3d().preWarmModels();
    }

    void MapSaveService::loadLightEntities(Map& map, const UdaChunk* sceneChunk, const UdaParser& udaParser) {
        auto lightsListChunk = udaParser.getUniqueChunk(true, LIGHTS_TAG, UdaAttribute(), sceneChunk);
        auto lightsChunk = udaParser.getChunks(LIGHT_TAG, UdaAttribute(), lightsListChunk);

        for (const auto& lightChunk : lightsChunk) {
            std::unique_ptr<LightEntity> lightEntity = LightEntityReaderWriter::load(lightChunk, udaParser);
            map.addLightEntity(std::move(lightEntity));
        }
    }

    void MapSaveService::loadTerrainEntities(Map& map, const UdaChunk* sceneChunk, const UdaParser& udaParser) {
        auto terrainsListChunk = udaParser.getUniqueChunk(true, TERRAINS_TAG, UdaAttribute(), sceneChunk);
        auto terrainsChunk = udaParser.getChunks(TERRAIN_TAG, UdaAttribute(), terrainsListChunk);

        for (const auto& terrainChunk : terrainsChunk) {
            std::unique_ptr<TerrainEntity> terrainEntity = TerrainEntityReaderWriter::load(terrainChunk, udaParser);
            map.addTerrainEntity(std::move(terrainEntity));
        }
    }

    void MapSaveService::loadWaterEntities(Map& map, const UdaChunk* sceneChunk, const UdaParser& udaParser) {
        auto watersListChunk = udaParser.getUniqueChunk(true, WATERS_TAG, UdaAttribute(), sceneChunk);
        auto watersChunk = udaParser.getChunks(WATER_TAG, UdaAttribute(), watersListChunk);

        for (const auto& waterChunk : watersChunk) {
            std::unique_ptr<WaterEntity> waterEntity = WaterEntityReaderWriter::load(waterChunk, udaParser);
            map.addWaterEntity(std::move(waterEntity));
        }
    }

    void MapSaveService::loadSkyEntity(Map& map, const UdaChunk* sceneChunk, const UdaParser& udaParser) {
        auto skyChunk = udaParser.getUniqueChunk(true, SKY_TAG, UdaAttribute(), sceneChunk);

        map.setSkyEntity(SkyEntityReaderWriter::load(skyChunk, udaParser));
    }

    void MapSaveService::loadSoundEntities(Map& map, const UdaChunk* sceneChunk, const UdaParser& udaParser) {
        auto soundElementsListChunk = udaParser.getUniqueChunk(true, SOUND_ELEMENTS_TAG, UdaAttribute(), sceneChunk);
        auto soundElementsChunk = udaParser.getChunks(SOUND_ELEMENT_TAG, UdaAttribute(), soundElementsListChunk);

        for (const auto& soundElementChunk : soundElementsChunk) {
            std::unique_ptr<SoundEntity> soundEntity = SoundEntityReaderWriter::load(soundElementChunk, udaParser);
            map.addSoundEntity(std::move(soundEntity));
        }
    }

    void MapSaveService::loadAIConfig(Map& map, const UdaChunk* sceneChunk, const UdaParser& udaParser) {
        auto aiElementsListChunk = udaParser.getUniqueChunk(true, AI_ELEMENTS_TAG, UdaAttribute(), sceneChunk);
        map.getAIEnvironment().getNavMeshGenerator().setNavMeshAgent(NavMeshAgentReaderWriter::loadNavMeshAgent(aiElementsListChunk, udaParser));
    }

    void MapSaveService::saveMap(const std::string& filename, Map& map) const {
        UdaWriter udaWriter(FileSystem::instance().getResourcesDirectory() + filename);

        auto& configChunk = udaWriter.createChunk(CONFIG_TAG);
        auto& workingDirChunk = udaWriter.createChunk(WORKING_DIR_TAG, UdaAttribute(), &configChunk);
        workingDirChunk.setStringValue(map.getRelativeWorkingDirectory());

        auto& sceneChunk = udaWriter.createChunk(SCENE_TAG);
        writeMap(map, sceneChunk, udaWriter);

        udaWriter.saveInFile();
    }

    void MapSaveService::writeMap(Map& map, UdaChunk& sceneChunk, UdaWriter& udaWriter) const {
        writeObjectEntities(map, sceneChunk, udaWriter);
        writeLightEntities(map, sceneChunk, udaWriter);
        writeTerrainEntities(map, sceneChunk, udaWriter);
        writeWaterEntities(map, sceneChunk, udaWriter);
        writeSkyEntity(map, sceneChunk, udaWriter);
        writeSoundEntities(map, sceneChunk, udaWriter);
        writeAIConfig(map, sceneChunk, udaWriter);
    }

    void MapSaveService::writeObjectEntities(Map& map, UdaChunk& sceneChunk, UdaWriter& udaWriter) const {
        auto& objectsListChunk = udaWriter.createChunk(OBJECTS_TAG, UdaAttribute(), &sceneChunk);

        for (auto& objectEntity : map.getObjectEntities()) {
            auto& objectsChunk = udaWriter.createChunk(OBJECT_TAG, UdaAttribute(), &objectsListChunk);
            ObjectEntityReaderWriter::write(objectsChunk, *objectEntity, udaWriter);
        }
    }

    void MapSaveService::writeLightEntities(Map& map, UdaChunk& sceneChunk, UdaWriter& udaWriter) const {
        auto& lightsListChunk = udaWriter.createChunk(LIGHTS_TAG, UdaAttribute(), &sceneChunk);

        for (auto& lightEntity : map.getLightEntities()) {
            auto& lightsChunk = udaWriter.createChunk(LIGHT_TAG, UdaAttribute(), &lightsListChunk);
            LightEntityReaderWriter::write(lightsChunk, *lightEntity, udaWriter);
        }
    }

    void MapSaveService::writeTerrainEntities(Map& map, UdaChunk& sceneChunk, UdaWriter& udaWriter) const {
        auto& terrainsListChunk = udaWriter.createChunk(TERRAINS_TAG, UdaAttribute(), &sceneChunk);

        for (auto& terrainEntity : map.getTerrainEntities()) {
            auto& terrainsChunk = udaWriter.createChunk(TERRAIN_TAG, UdaAttribute(), &terrainsListChunk);
            TerrainEntityReaderWriter::write(terrainsChunk, *terrainEntity, udaWriter);
        }
    }

    void MapSaveService::writeWaterEntities(Map& map, UdaChunk& sceneChunk, UdaWriter& udaWriter) const {
        auto& watersListChunk = udaWriter.createChunk(WATERS_TAG, UdaAttribute(), &sceneChunk);

        for (auto& waterEntity : map.getWaterEntities()) {
            auto& watersChunk = udaWriter.createChunk(WATER_TAG, UdaAttribute(), &watersListChunk);
            WaterEntityReaderWriter::write(watersChunk, *waterEntity, udaWriter);
        }
    }

    void MapSaveService::writeSkyEntity(Map& map, UdaChunk& sceneChunk, UdaWriter& udaWriter) const {
        auto& skyChunk = udaWriter.createChunk(SKY_TAG, UdaAttribute(), &sceneChunk);

        SkyEntityReaderWriter::write(skyChunk, map.getSkyEntity(), udaWriter);
    }

    void MapSaveService::writeSoundEntities(Map& map, UdaChunk& sceneChunk, UdaWriter& udaWriter) const {
        auto& soundElementsListChunk = udaWriter.createChunk(SOUND_ELEMENTS_TAG, UdaAttribute(), &sceneChunk);

        for (auto& soundEntity : map.getSoundEntities()) {
            auto& soundElementsChunk = udaWriter.createChunk(SOUND_ELEMENT_TAG, UdaAttribute(), &soundElementsListChunk);
            SoundEntityReaderWriter::write(soundElementsChunk, *soundEntity, udaWriter);
        }
    }

    void MapSaveService::writeAIConfig(Map& map, UdaChunk& sceneChunk, UdaWriter& udaWriter) const {
        auto& aiElementsListChunk = udaWriter.createChunk(AI_ELEMENTS_TAG, UdaAttribute(), &sceneChunk);
        NavMeshAgentReaderWriter::writeNavMeshAgent(aiElementsListChunk, map.getAIEnvironment().getNavMeshGenerator().getNavMeshAgent(), udaWriter);
    }

    /**
     * @param filename Absolute path to file containing map information
     * @return Working directory relative to the map file
     */
    std::string MapSaveService::getRelativeWorkingDirectory(const std::string& filename) {
        UdaParser udaParser(filename);
        const UdaChunk* configChunk = udaParser.getUniqueChunk(true, CONFIG_TAG);
        const UdaChunk* workingDirChunk = udaParser.getUniqueChunk(true, WORKING_DIR_TAG, UdaAttribute(), configChunk);

        return workingDirChunk->getStringValue();
    }

}