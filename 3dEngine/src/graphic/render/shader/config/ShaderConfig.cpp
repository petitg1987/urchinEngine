#include <graphic/render/shader/config/ShaderConfig.h>

namespace urchin {

    ShaderConfig::ShaderConfig() :
            Singleton<ShaderConfig>(),
            shadersDirectoryName(ConfigService::instance()->getStringValue("shaders.shadersLocation")) {
        this->shadersParentDirectory = FileSystem::instance()->getResourcesDirectory();
    }

    void ShaderConfig::replaceShadersParentDirectoryBy(const std::string& shadersParentDirectory) {
        this->shadersParentDirectory = shadersParentDirectory;
    }

    std::string ShaderConfig::getShadersDirectory() const {
        return shadersParentDirectory + shadersDirectoryName;
    }

}
