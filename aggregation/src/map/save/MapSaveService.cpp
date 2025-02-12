#include <stdexcept>
#include <Urchin3dEngine.h>
#include <UrchinAIEngine.h>

#include <map/save/MapSaveService.h>
#include <map/save/ai/NavMeshAgentEntityReaderWriter.h>
#include <map/save/object/ObjectEntityReaderWriter.h>
#include <map/save/terrain/TerrainEntityReaderWriter.h>
#include <map/save/object/sound/SoundEntityReaderWriter.h>
#include <map/save/water/WaterEntityReaderWriter.h>
#include <map/save/sky/SkyEntityReaderWriter.h>

namespace urchin {

    /**
     * @param map [out] Map to load the entities (must be empty)
     */
    void MapSaveService::loadMap(const std::string& filename, LoadMapCallback& loadMapCallback, Map& map) const {
        Logger::instance().logInfo("Load map: " + filename);

        std::string mapPath = FileUtil::isAbsolutePath(filename) ? filename : FileSystem::instance().getResourcesDirectory() + filename;
        UdaParser udaParser(mapPath, UdaLoadType::LOAD_FILE);

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

        loadMapCallback.notify(LoadMapCallback::OBJECTS, LoadMapCallback::START_LOADING);
        loadObjectEntities(map, sceneChunk, udaParser);
        loadMapCallback.notify(LoadMapCallback::OBJECTS, LoadMapCallback::LOADED);

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
        std::string mapPath = FileUtil::isAbsolutePath(filename) ? filename : FileSystem::instance().getResourcesDirectory() + filename;
        UdaParser udaParser(mapPath, UdaLoadType::NO_LOAD);

        auto& configChunk = udaParser.createChunk(CONFIG_TAG);
        auto& workingDirChunk = udaParser.createChunk(WORKING_DIR_TAG, UdaAttribute(), &configChunk);
        workingDirChunk.setStringValue(map.getRelativeWorkingDirectory());

        auto& sceneChunk = udaParser.createChunk(SCENE_TAG);
        writeMap(map, sceneChunk, udaParser);

        udaParser.save();
    }

    void MapSaveService::writeMap(const Map& map, UdaChunk& sceneChunk, UdaParser& udaParser) const {
        writeObjectEntities(map, sceneChunk, udaParser);
        writeTerrainEntities(map, sceneChunk, udaParser);
        writeWaterEntities(map, sceneChunk, udaParser);
        writeSkyEntity(map, sceneChunk, udaParser);
        writeSoundEntities(map, sceneChunk, udaParser);
        writeAIConfig(map, sceneChunk, udaParser);
    }

    void MapSaveService::writeObjectEntities(const Map& map, UdaChunk& sceneChunk, UdaParser& udaParser) const {
        auto& objectsListChunk = udaParser.createChunk(OBJECTS_TAG, UdaAttribute(), &sceneChunk);

        for (auto& objectEntity : map.getObjectEntities()) {
            auto& objectsChunk = udaParser.createChunk(OBJECT_TAG, UdaAttribute(), &objectsListChunk);
            ObjectEntityReaderWriter::write(objectsChunk, *objectEntity, udaParser);
        }
    }

    void MapSaveService::writeTerrainEntities(const Map& map, UdaChunk& sceneChunk, UdaParser& udaParser) const {
        auto& terrainsListChunk = udaParser.createChunk(TERRAINS_TAG, UdaAttribute(), &sceneChunk);

        for (auto& terrainEntity : map.getTerrainEntities()) {
            auto& terrainsChunk = udaParser.createChunk(TERRAIN_TAG, UdaAttribute(), &terrainsListChunk);
            TerrainEntityReaderWriter::write(terrainsChunk, *terrainEntity, udaParser);
        }
    }

    void MapSaveService::writeWaterEntities(const Map& map, UdaChunk& sceneChunk, UdaParser& udaParser) const {
        auto& watersListChunk = udaParser.createChunk(WATERS_TAG, UdaAttribute(), &sceneChunk);

        for (auto& waterEntity : map.getWaterEntities()) {
            auto& watersChunk = udaParser.createChunk(WATER_TAG, UdaAttribute(), &watersListChunk);
            WaterEntityReaderWriter::write(watersChunk, *waterEntity, udaParser);
        }
    }

    void MapSaveService::writeSkyEntity(const Map& map, UdaChunk& sceneChunk, UdaParser& udaParser) const {
        auto& skyChunk = udaParser.createChunk(SKY_TAG, UdaAttribute(), &sceneChunk);

        SkyEntityReaderWriter::write(skyChunk, map.getSkyEntity(), udaParser);
    }

    void MapSaveService::writeSoundEntities(const Map& map, UdaChunk& sceneChunk, UdaParser& udaParser) const {
        auto& soundElementsListChunk = udaParser.createChunk(SOUND_ELEMENTS_TAG, UdaAttribute(), &sceneChunk);

        for (auto& soundEntity : map.getSoundEntities()) {
            auto& soundElementsChunk = udaParser.createChunk(SOUND_ELEMENT_TAG, UdaAttribute(), &soundElementsListChunk);
            SoundEntityReaderWriter::write(soundElementsChunk, *soundEntity, udaParser);
        }
    }

    void MapSaveService::writeAIConfig(const Map& map, UdaChunk& sceneChunk, UdaParser& udaParser) const {
        auto& aiElementsListChunk = udaParser.createChunk(AI_ELEMENTS_TAG, UdaAttribute(), &sceneChunk);

        NavMeshAgentEntityReaderWriter::write(aiElementsListChunk, map.getNavMeshAgentEntity(), udaParser);
    }

    /**
     * @param filename Absolute path to file containing map information
     * @return Working directory relative to the map file
     */
    std::string MapSaveService::getRelativeWorkingDirectory(std::string filename) {
        UdaParser udaParser(std::move(filename), UdaLoadType::LOAD_FILE);
        const UdaChunk* configChunk = udaParser.getFirstChunk(true, CONFIG_TAG);
        const UdaChunk* workingDirChunk = udaParser.getFirstChunk(true, WORKING_DIR_TAG, UdaAttribute(), configChunk);

        return workingDirChunk->getStringValue();
    }

}
