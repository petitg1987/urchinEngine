#pragma once

#include <string>

namespace urchin {

    class FileSystem {
        public:
            static FileSystem& instance();

            void setupResourcesDirectory(std::string);
            const std::string& getResourcesDirectory() const;

        private:
            FileSystem();

            std::string resourcesDirectory;
    };

}
