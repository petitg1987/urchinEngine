#pragma once

#include <string>

namespace urchin {

    class FileUtil {
        public:
            static bool isDirectoryExist(const std::string&);
            static bool isFileExist(const std::string&);

            static void createDirectory(const std::string&);
            static void copyDirectoryContent(const std::string&, const std::string&);
            static void copyFile(const std::string&, const std::string&);
            static void deleteDirectory(const std::string&);

            static std::string getFileExtension(const std::string&);
            static std::string getFileName(const std::string&);
            static std::string getFileNameNoExtension(const std::string&);
            static std::string getDirectory(const std::string&);
            static std::string getRelativePath(const std::string&, const std::string&);
            static std::string simplifyDirectoryPath(const std::string&);

        private:
            FileUtil() = default;
            ~FileUtil() = default;

            static void checkDirectory(const std::string&);
    };

}
