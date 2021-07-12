#pragma once

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

            std::string shadersParentDirectory;
            std::string shadersDirectoryName;
    };

}
