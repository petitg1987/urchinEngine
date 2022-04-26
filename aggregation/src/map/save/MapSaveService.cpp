#include <stdexcept>

#include <map/save/MapSaveService.h>
#include <map/save/ai/NavMeshAgentEntityReaderWriter.h>
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
    void MapSaveService::loadMap(const std::string& filename, LoadMapCallback& loadMapCallback, Map& map) const {
        Logger::instance().logInfo("Load map: " + filename);

        std::string mapPath;
        if (FileUtil::isAbsolutePath(filename)) {
            mapPath = filename;
        } else {
            mapPath = FileSystem::instance().getResourcesDirectory() + filename;
        }
        UdaParser udaParser(mapPath);

        const UdaChunk* configChunk = udaParser.getFirstChunk(true, CONFIG_TAG);
        const UdaChunk* workingDirChunk = udaParser.getFirstChunk(true, WORKING_DIR_TAG, UdaAttribute(), configChunk);
        map.setRelativeWorkingDirectory(workingDirChunk->getStringValue());

        const UdaChunk* sceneChunk = udaParser.getFirstChunk(true, SCENE_TAG);
        loadMap(map, sceneChunk, udaParser, loadMapCallback);
    }

    void MapSaveService::loadMap(Map& map, const UdaChunk* sceneChunk, const UdaParser& udaParser, LoadMapCallback& loadMapCallback) const {
        if (map.getRenderer3d() && !map.getRenderer3d()->isPaused()) { //to avoid move camera before being able to see the map
            throw std::runtime_error("Renderer 3d should be paused while loading map.");
        }
        if (map.getPhysicsWorld() && !map.getPhysicsWorld()->isPaused()) { //to avoid miss of collision between objects just loaded and on objects not loaded yet
            throw std::runtime_error("Physics world should be paused while loading map.");
        }
        if (map.getAIEnvironment() && !map.getAIEnvironment()->isPaused()) { //to avoid compute path based on a world with missing objects
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

    void MapSaveService::loadObjectEntities(Map& map, const UdaChunk* sceneChunk, const UdaParser& udaParser) const {
        auto objectsListChunk = udaParser.getFirstChunk(true, OBJECTS_TAG, UdaAttribute(), sceneChunk);
        auto objectsChunk = udaParser.getChunks(OBJECT_TAG, UdaAttribute(), objectsListChunk);

        for (const auto& objectChunk : objectsChunk) {
            map.addObjectEntity(ObjectEntityReaderWriter::load(objectChunk, udaParser));
        }
        if (map.getRenderer3d()) {
            map.getRenderer3d()->preWarmModels();
        }
    }

    void MapSaveService::loadLightEntities(Map& map, const UdaChunk* sceneChunk, const UdaParser& udaParser) const {
        auto lightsListChunk = udaParser.getFirstChunk(true, LIGHTS_TAG, UdaAttribute(), sceneChunk);
        auto lightsChunk = udaParser.getChunks(LIGHT_TAG, UdaAttribute(), lightsListChunk);

        for (const auto& lightChunk : lightsChunk) {
            map.addLightEntity(LightEntityReaderWriter::load(lightChunk, udaParser));
        }
    }

    void MapSaveService::loadTerrainEntities(Map& map, const UdaChunk* sceneChunk, const UdaParser& udaParser) const {
        auto terrainsListChunk = udaParser.getFirstChunk(true, TERRAINS_TAG, UdaAttribute(), sceneChunk);
        auto terrainsChunk = udaParser.getChunks(TERRAIN_TAG, UdaAttribute(), terrainsListChunk);

        for (const auto& terrainChunk : terrainsChunk) {
            map.addTerrainEntity(TerrainEntityReaderWriter::load(terrainChunk, udaParser));
        }
    }

    void MapSaveService::loadWaterEntities(Map& map, const UdaChunk* sceneChunk, const UdaParser& udaParser) const {
        auto watersListChunk = udaParser.getFirstChunk(true, WATERS_TAG, UdaAttribute(), sceneChunk);
        auto watersChunk = udaParser.getChunks(WATER_TAG, UdaAttribute(), watersListChunk);

        for (const auto& waterChunk : watersChunk) {
            map.addWaterEntity(WaterEntityReaderWriter::load(waterChunk, udaParser));
        }
    }

    void MapSaveService::loadSkyEntity(Map& map, const UdaChunk* sceneChunk, const UdaParser& udaParser) const {
        auto skyChunk = udaParser.getFirstChunk(true, SKY_TAG, UdaAttribute(), sceneChunk);

        map.setSkyEntity(SkyEntityReaderWriter::load(skyChunk, udaParser));
    }

    void MapSaveService::loadSoundEntities(Map& map, const UdaChunk* sceneChunk, const UdaParser& udaParser) const {
        auto soundElementsListChunk = udaParser.getFirstChunk(true, SOUND_ELEMENTS_TAG, UdaAttribute(), sceneChunk);
        auto soundElementsChunk = udaParser.getChunks(SOUND_ELEMENT_TAG, UdaAttribute(), soundElementsListChunk);

        for (const auto& soundElementChunk : soundElementsChunk) {
            map.addSoundEntity(SoundEntityReaderWriter::load(soundElementChunk, udaParser));
        }
    }

    void MapSaveService::loadAIConfig(Map& map, const UdaChunk* sceneChunk, const UdaParser& udaParser) const {
        auto aiElementsListChunk = udaParser.getFirstChunk(true, AI_ELEMENTS_TAG, UdaAttribute(), sceneChunk);

        map.setNavMeshAgentEntity(NavMeshAgentEntityReaderWriter::load(aiElementsListChunk, udaParser));
    }

    void MapSaveService::saveMap(const std::string& filename, const Map& map) const {
        UdaWriter udaWriter(FileSystem::instance().getResourcesDirectory() + filename);

        auto& configChunk = udaWriter.createChunk(CONFIG_TAG);
        auto& workingDirChunk = udaWriter.createChunk(WORKING_DIR_TAG, UdaAttribute(), &configChunk);
        workingDirChunk.setStringValue(map.getRelativeWorkingDirectory());

        auto& sceneChunk = udaWriter.createChunk(SCENE_TAG);
        writeMap(map, sceneChunk, udaWriter);

        udaWriter.saveInFile();
    }

    void MapSaveService::writeMap(const Map& map, UdaChunk& sceneChunk, UdaWriter& udaWriter) const {
        writeObjectEntities(map, sceneChunk, udaWriter);
        writeLightEntities(map, sceneChunk, udaWriter);
        writeTerrainEntities(map, sceneChunk, udaWriter);
        writeWaterEntities(map, sceneChunk, udaWriter);
        writeSkyEntity(map, sceneChunk, udaWriter);
        writeSoundEntities(map, sceneChunk, udaWriter);
        writeAIConfig(map, sceneChunk, udaWriter);
    }

    void MapSaveService::writeObjectEntities(const Map& map, UdaChunk& sceneChunk, UdaWriter& udaWriter) const {
        auto& objectsListChunk = udaWriter.createChunk(OBJECTS_TAG, UdaAttribute(), &sceneChunk);

        for (auto& objectEntity : map.getObjectEntities()) {
            auto& objectsChunk = udaWriter.createChunk(OBJECT_TAG, UdaAttribute(), &objectsListChunk);
            ObjectEntityReaderWriter::write(objectsChunk, *objectEntity, udaWriter);
        }
    }

    void MapSaveService::writeLightEntities(const Map& map, UdaChunk& sceneChunk, UdaWriter& udaWriter) const {
        auto& lightsListChunk = udaWriter.createChunk(LIGHTS_TAG, UdaAttribute(), &sceneChunk);

        for (auto& lightEntity : map.getLightEntities()) {
            auto& lightsChunk = udaWriter.createChunk(LIGHT_TAG, UdaAttribute(), &lightsListChunk);
            LightEntityReaderWriter::write(lightsChunk, *lightEntity, udaWriter);
        }
    }

    void MapSaveService::writeTerrainEntities(const Map& map, UdaChunk& sceneChunk, UdaWriter& udaWriter) const {
        auto& terrainsListChunk = udaWriter.createChunk(TERRAINS_TAG, UdaAttribute(), &sceneChunk);

        for (auto& terrainEntity : map.getTerrainEntities()) {
            auto& terrainsChunk = udaWriter.createChunk(TERRAIN_TAG, UdaAttribute(), &terrainsListChunk);
            TerrainEntityReaderWriter::write(terrainsChunk, *terrainEntity, udaWriter);
        }
    }

    void MapSaveService::writeWaterEntities(const Map& map, UdaChunk& sceneChunk, UdaWriter& udaWriter) const {
        auto& watersListChunk = udaWriter.createChunk(WATERS_TAG, UdaAttribute(), &sceneChunk);

        for (auto& waterEntity : map.getWaterEntities()) {
            auto& watersChunk = udaWriter.createChunk(WATER_TAG, UdaAttribute(), &watersListChunk);
            WaterEntityReaderWriter::write(watersChunk, *waterEntity, udaWriter);
        }
    }

    void MapSaveService::writeSkyEntity(const Map& map, UdaChunk& sceneChunk, UdaWriter& udaWriter) const {
        auto& skyChunk = udaWriter.createChunk(SKY_TAG, UdaAttribute(), &sceneChunk);

        SkyEntityReaderWriter::write(skyChunk, map.getSkyEntity(), udaWriter);
    }

    void MapSaveService::writeSoundEntities(const Map& map, UdaChunk& sceneChunk, UdaWriter& udaWriter) const {
        auto& soundElementsListChunk = udaWriter.createChunk(SOUND_ELEMENTS_TAG, UdaAttribute(), &sceneChunk);

        for (auto& soundEntity : map.getSoundEntities()) {
            auto& soundElementsChunk = udaWriter.createChunk(SOUND_ELEMENT_TAG, UdaAttribute(), &soundElementsListChunk);
            SoundEntityReaderWriter::write(soundElementsChunk, *soundEntity, udaWriter);
        }
    }

    void MapSaveService::writeAIConfig(const Map& map, UdaChunk& sceneChunk, UdaWriter& udaWriter) const {
        auto& aiElementsListChunk = udaWriter.createChunk(AI_ELEMENTS_TAG, UdaAttribute(), &sceneChunk);

        NavMeshAgentEntityReaderWriter::write(aiElementsListChunk, map.getNavMeshAgentEntity(), udaWriter);
    }

    /**
     * @param filename Absolute path to file containing map information
     * @return Working directory relative to the map file
     */
    std::string MapSaveService::getRelativeWorkingDirectory(std::string filename) {
        UdaParser udaParser(std::move(filename));
        const UdaChunk* configChunk = udaParser.getFirstChunk(true, CONFIG_TAG);
        const UdaChunk* workingDirChunk = udaParser.getFirstChunk(true, WORKING_DIR_TAG, UdaAttribute(), configChunk);

        return workingDirChunk->getStringValue();
    }

}
