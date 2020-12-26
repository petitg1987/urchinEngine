#ifndef URCHINENGINE_FILEUTIL_H
#define URCHINENGINE_FILEUTIL_H

#include <string>

#include "pattern/singleton/Singleton.h"

namespace urchin {

    class FileUtil : public Singleton<FileUtil> {
        public:
            friend class Singleton<FileUtil>;

            bool isDirectoryExist(const std::string&);
            bool isFileExist(const std::string&);

            void createDirectory(const std::string&);
            void copyDirectoryContent(const std::string&, const std::string&);

            std::string getFileExtension(const std::string&);
            std::string getFileName(const std::string&);
            std::string getFileNameNoExtension(const std::string&);
            std::string getDirectoryFrom(const std::string&);
            std::string getRelativePath(const std::string&, const std::string&);
            std::string simplifyDirectoryPath(const std::string&);

        private:
            FileUtil() = default;
            ~FileUtil() override = default;

            void checkDirectory(const std::string&);
    };

}

#endif
