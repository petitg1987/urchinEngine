#include <graphics/render/shader/ShaderConfig.h>

namespace urchin {

    ShaderConfig::ShaderConfig() :
            Singleton(),
            shadersParentDirectory(FileSystem::instance().getResourcesDirectory()),
            shadersDirectoryName(ConfigService::instance().getStringValue("shaders.shadersLocation")) {

    }

    void ShaderConfig::replaceShadersParentDirectoryBy(std::string shadersParentDirectory) {
        this->shadersParentDirectory = std::move(shadersParentDirectory);
    }

    std::string ShaderConfig::getShadersDirectory() const {
        return shadersParentDirectory + shadersDirectoryName;
    }

}
