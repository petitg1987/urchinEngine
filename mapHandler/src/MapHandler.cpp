#include <stdexcept>

#include <MapHandler.h>

namespace urchin {

    MapHandler::MapHandler() {
        SignalHandler::instance().initialize(); //TODO move somewhere else ?
    }

    /**
     * @param map [out] Map to load the entities (must be empty)
     */
    void MapHandler::loadMapFromFile(const std::string& filename, LoadMapCallback& loadMapCallback, const std::unique_ptr<Map>& map) {
        Logger::instance().logInfo("Load map: " + filename);
        UdaParser udaParser(FileSystem::instance().getResourcesDirectory() + filename);

        const UdaChunk* configChunk = udaParser.getUniqueChunk(true, CONFIG_TAG);
        const UdaChunk* workingDirChunk = udaParser.getUniqueChunk(true, WORKING_DIR_TAG, UdaAttribute(), configChunk);
        map->setRelativeWorkingDirectory(workingDirChunk->getStringValue());

        const UdaChunk* sceneChunk = udaParser.getUniqueChunk(true, SCENE_TAG);
        map->loadFrom(sceneChunk, udaParser, loadMapCallback);
    }

    void MapHandler::writeMapOnFile(const std::string& filename, Map& map) const {
        UdaWriter udaWriter(FileSystem::instance().getResourcesDirectory() + filename);

        auto& configChunk = udaWriter.createChunk(CONFIG_TAG);
        auto& workingDirChunk = udaWriter.createChunk(WORKING_DIR_TAG, UdaAttribute(), &configChunk);
        workingDirChunk.setStringValue(map.getRelativeWorkingDirectory());

        auto& sceneChunk = udaWriter.createChunk(SCENE_TAG);
        map.writeOn(sceneChunk, udaWriter);

        udaWriter.saveInFile();
    }

    /**
     * @param filename Absolute path to file containing map information
     * @return Working directory relative to the map file
     */
    std::string MapHandler::getRelativeWorkingDirectory(const std::string& filename) {
        UdaParser udaParser(filename);
        const UdaChunk* configChunk = udaParser.getUniqueChunk(true, CONFIG_TAG);
        const UdaChunk* workingDirChunk = udaParser.getUniqueChunk(true, WORKING_DIR_TAG, UdaAttribute(), configChunk);

        return workingDirChunk->getStringValue();
    }

}
