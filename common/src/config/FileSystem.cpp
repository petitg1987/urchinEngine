#include <config/FileSystem.h>
#include <system/SystemInfo.h>
#include <util/FileUtil.h>

namespace urchin {

    FileSystem::FileSystem() :
            resourcesDirectory("./") {
        this->engineUserDataDirectory = SystemInfo::userDataDirectory() + getEngineDirectoryName() + "/";
        FileUtil::createDirectory(engineUserDataDirectory);

        this->engineUserCacheDirectory = engineUserDataDirectory + "cache/";
        FileUtil::createDirectory(engineUserCacheDirectory);
    }

    void FileSystem::setupResourcesDirectory(std::string resourcesDirectory) {
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
            return ".urchin-engine";
        #endif
    }

}
