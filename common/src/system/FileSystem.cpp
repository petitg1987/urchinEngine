#include "system/FileSystem.h"

namespace urchin {

    FileSystem::FileSystem() : Singleton<FileSystem>(),
            resourcesDirectory("./"),
            saveDirectory("./") {

    }

    void FileSystem::setupResourcesDirectory(const std::string &resourcesDirectory) {
        this->resourcesDirectory = resourcesDirectory;
    }

    const std::string &FileSystem::getResourcesDirectory() const {
        return resourcesDirectory;
    }

    void FileSystem::setupSaveDirectory(const std::string &saveDirectory) {
        this->saveDirectory = saveDirectory;
    }

    const std::string &FileSystem::getSaveDirectory() const {
        return saveDirectory;
    }

}
