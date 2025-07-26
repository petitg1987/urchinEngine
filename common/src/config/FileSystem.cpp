#include "config/FileSystem.h"
#include "system/SystemInfo.h"
#include "util/FileUtil.h"

namespace urchin {

    FileSystem& FileSystem::instance() {
        static FileSystem instance;
        return instance;
    }

    FileSystem::FileSystem() :
            resourcesDirectory("./") {

    }

    void FileSystem::setupResourcesDirectory(std::string resourcesDirectory) {
        if (!FileUtil::isAbsolutePath(resourcesDirectory)) {
            throw std::runtime_error("Resources directory must be defined in absolute path: " + resourcesDirectory);
        }
        this->resourcesDirectory = std::move(resourcesDirectory);
    }

    const std::string& FileSystem::getResourcesDirectory() const {
        return resourcesDirectory;
    }

}
