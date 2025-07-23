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
        this->engineUserDataDirectory = SystemInfo::userDataDirectory() + getEngineDirectoryName() + "/";
        FileUtil::createDirectory(engineUserDataDirectory);

        this->engineUserCacheDirectory = engineUserDataDirectory + "cache/";
        FileUtil::createDirectory(engineUserCacheDirectory);
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

    const std::string& FileSystem::getEngineUserDataDirectory() const {
        return engineUserDataDirectory;
    }

    const std::string& FileSystem::getEngineUserCacheDirectory() const {
        return engineUserCacheDirectory;
    }

    std::string FileSystem::getEngineDirectoryName() const {
        #ifdef _WIN32
            return "UrchinEngine";
        #else
            return "urchin-engine";
        #endif
    }

}
