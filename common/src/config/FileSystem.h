#pragma once

#include <string>

namespace urchin {

    class FileSystem {
        public:
            static FileSystem& instance();

            void setupResourcesDirectory(std::string);
            const std::string& getResourcesDirectory() const;

            const std::string& getEngineUserDataDirectory() const;
            const std::string& getEngineUserCacheDirectory() const;

        private:
            FileSystem();

            std::string getEngineDirectoryName() const;

            std::string resourcesDirectory;

            std::string engineUserDataDirectory;
            std::string engineUserCacheDirectory;
    };

}
