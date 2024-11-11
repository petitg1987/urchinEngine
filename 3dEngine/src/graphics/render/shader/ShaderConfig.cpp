#include <graphics/render/shader/ShaderConfig.h>

namespace urchin {

    ShaderConfig& ShaderConfig::instance() {
        static ShaderConfig instance;
        return instance;
    }

    ShaderConfig::ShaderConfig() :
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
