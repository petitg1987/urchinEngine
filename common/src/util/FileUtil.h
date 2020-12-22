#ifndef URCHINENGINE_FILEHANDLER_H
#define URCHINENGINE_FILEHANDLER_H

#include <string>

namespace urchin {

    class FileUtil {
        public:
            static std::string getFileExtension(const std::string&);
            static std::string getFileName(const std::string&);
            static std::string getFileNameNoExtension(const std::string&);
            static std::string getDirectoryFrom(const std::string&);
            static std::string getRelativePath(const std::string&, const std::string&);
            static std::string simplifyDirectoryPath(const std::string&);

        private:
            FileUtil() = default;
            ~FileUtil() = default;

            static void checkDirectory(const std::string&);
    };

}

#endif
