#ifndef URCHINENGINE_FILESYSTEM_H
#define URCHINENGINE_FILESYSTEM_H

#include <list>

#include "pattern/singleton/Singleton.h"

namespace urchin {

    class FileSystem : public Singleton<FileSystem> {
        public:
            friend class Singleton<FileSystem>;

            void setupResourcesDirectory(const std::string&);
            const std::string& getResourcesDirectory() const;

            const std::string& getEngineUserDataDirectory() const;
            const std::string& getEngineUserCacheDirectory() const;

        private:
            FileSystem();
            ~FileSystem() override = default;

            static std::string getEngineDirectoryName();

            std::string resourcesDirectory;

            std::string engineUserDataDirectory;
            std::string engineUserCacheDirectory;
    };

}

#endif
