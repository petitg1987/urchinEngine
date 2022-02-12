#pragma once

#include <string>
#include <vector>

namespace urchin {

    class FileUtil {
        public:
            static bool isDirectoryExist(const std::string&);
            static bool isFileExist(const std::string&);
            static long getFileSize(const std::string&);

            static void createDirectory(std::string_view);
            static void createEmptyFile(const std::string&);
            static void copyDirectoryContent(std::string_view, const std::string&);
            static std::vector<std::string> getFilesRecursive(const std::string&);
            static void copyFile(const std::string&, const std::string&);
            static void deleteDirectory(std::string_view);

            static std::string_view getFileExtension(std::string_view);
            static std::string_view getFileName(std::string_view);
            static std::string_view getFileNameNoExtension(std::string_view);
            static std::string_view getDirectory(std::string_view);
            static std::string getRelativePath(const std::string&, const std::string&);
            static std::string simplifyDirectoryPath(const std::string&);

        private:
            FileUtil() = default;
            ~FileUtil() = default;

            static void getFilesRecursive(std::string_view, std::vector<std::string>&);
            static void checkDirectory(std::string_view);
    };

}
