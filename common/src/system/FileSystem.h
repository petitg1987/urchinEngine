#ifndef URCHINENGINE_FILESYSTEM_H
#define URCHINENGINE_FILESYSTEM_H

#include <list>

#include "pattern/singleton/Singleton.h"

namespace urchin {

    class FileSystem : public Singleton<FileSystem> {
        public:
            friend class Singleton<FileSystem>;

            void setupResourcesDirectory(const std::string &);
            const std::string &getResourcesDirectory() const;

            void setupSaveDirectory(const std::string &);
            const std::string &getSaveDirectory() const;

        private:
            FileSystem();
            ~FileSystem() override = default;

            std::string resourcesDirectory;
            std::string saveDirectory;
    };

}

#endif
