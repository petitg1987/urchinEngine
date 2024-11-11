#pragma once

#include <string>

namespace urchin {

    class ShaderConfig {
        public:
            static ShaderConfig& instance();

            void replaceShadersParentDirectoryBy(std::string);

            std::string getShadersDirectory() const;

        private:
            ShaderConfig();

            std::string shadersParentDirectory;
            std::string shadersDirectoryName;
    };

}
