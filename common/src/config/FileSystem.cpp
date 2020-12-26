#include <filesystem>

#include "config/FileSystem.h"
#include "system/SystemInfo.h"
#include "util/FileUtil.h"

namespace urchin {

    FileSystem::FileSystem() : Singleton<FileSystem>(),
            resourcesDirectory("./") {

        this->engineUserDataDirectory = SystemInfo::instance()->userDataDirectory() + "/" + getEngineDirectoryName();
        FileUtil::createDirectory(engineUserDataDirectory);

        this->engineUserCacheDirectory = engineUserDataDirectory + "/cache";
        FileUtil::createDirectory(engineUserCacheDirectory);
    }

    void FileSystem::setupResourcesDirectory(const std::string& resourcesDirectory) {
        this->resourcesDirectory = resourcesDirectory;
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
            return ".urchin-engine";
        #endif
    }

}
