#ifndef URCHINENGINE_CONFIG_H
#define URCHINENGINE_CONFIG_H

#include <string>
#include <UrchinCommon.h>

namespace urchin {

    class ShaderConfig : public Singleton<ShaderConfig> {
        public:
            friend class Singleton<ShaderConfig>;

            void replaceShadersParentDirectoryBy(const std::string&);

            std::string getShadersDirectory() const;

        private:
            ShaderConfig();
            ~ShaderConfig() override = default;

            std::string shadersParentDirectory;
            std::string shadersDirectoryName;
    };

}

#endif
