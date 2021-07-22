#include <stdexcept>

#include "MapHandler.h"
#include <load/NullLoadMapCallback.h>

namespace urchin {

    MapHandler::MapHandler(Renderer3d* renderer3d, PhysicsWorld* physicsWorld, SoundEnvironment* soundEnvironment, AIEnvironment* aiEnvironment) :
            map(nullptr) {
        SignalHandler::instance().initialize();

        if (!renderer3d) {
            throw std::invalid_argument("Renderer 3d cannot be null in map handler");
        }

        if (!soundEnvironment) {
            throw std::invalid_argument("Sound environment cannot be null in map handler");
        }

        if (!aiEnvironment) {
            throw std::invalid_argument("AI environment cannot be null in map handler");
        }

        map = std::make_unique<Map>(renderer3d, physicsWorld, soundEnvironment, aiEnvironment);
    }

    void MapHandler::loadMapFromFile(const std::string& filename, LoadMapCallback& loadMapCallback) {
        Logger::instance().logInfo("Load map: " + filename);
        UdaParser udaParser(FileSystem::instance().getResourcesDirectory() + filename);

        UdaChunk* configChunk = udaParser.getUniqueChunk(true, CONFIG_TAG);
        UdaChunk* workingDirChunk = udaParser.getUniqueChunk(true, WORKING_DIR_TAG, UdaAttribute(), configChunk);
        relativeWorkingDirectory = workingDirChunk->getStringValue();

        UdaChunk* sceneChunk = udaParser.getUniqueChunk(true, SCENE_TAG);
        map->loadFrom(sceneChunk, udaParser, loadMapCallback);
    }

    void MapHandler::writeMapOnFile(const std::string& filename) const {
        UdaWriter udaWriter(FileSystem::instance().getResourcesDirectory() + filename);

        auto& configChunk = udaWriter.createChunk(CONFIG_TAG);
        auto& workingDirChunk = udaWriter.createChunk(WORKING_DIR_TAG, UdaAttribute(), &configChunk);
        workingDirChunk.setStringValue(relativeWorkingDirectory);

        auto& sceneChunk = udaWriter.createChunk(SCENE_TAG);
        map->writeOn(sceneChunk, udaWriter);

        udaWriter.saveInFile();
    }

    /**
     * @param filename Absolute path to file containing map information
     * @return Working directory relative to the map file
     */
    std::string MapHandler::getRelativeWorkingDirectory(const std::string& filename) {
        UdaParser udaParser(filename);
        UdaChunk* configChunk = udaParser.getUniqueChunk(true, CONFIG_TAG);
        UdaChunk* workingDirChunk = udaParser.getUniqueChunk(true, WORKING_DIR_TAG, UdaAttribute(), configChunk);

        return workingDirChunk->getStringValue();
    }

    /**
     * @return Working directory relative to the map file
     */
    std::string MapHandler::getRelativeWorkingDirectory() const {
        return relativeWorkingDirectory;
    }

    /**
     * @param relativeWorkingDirectory Relative working directory to the map file
     */
    void MapHandler::setRelativeWorkingDirectory(const std::string& relativeWorkingDirectory) {
        this->relativeWorkingDirectory = relativeWorkingDirectory;
    }

    void MapHandler::refreshMap() {
        map->refreshMap();
    }

    Map& MapHandler::getMap() const {
        return *map;
    }

    void MapHandler::pause() {
        map->pause();
    }

    void MapHandler::unpause() {
        map->unpause();
    }

}
