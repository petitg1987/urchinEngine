#pragma once

#include <string>
#include <vector>
#include <filesystem>

namespace urchin {

    class FileUtil {
        public:
            static bool isDirectoryExist(const std::string&);
            static bool isFileExist(const std::string&);
            static long getFileSize(const std::string&);

            static void createDirectory(std::string_view);
            static void createEmptyFile(const std::string&);
            static void copyDirectoryContent(std::string_view, const std::string&);
            static std::vector<std::string> getFiles(std::string_view);
            static std::vector<std::string> getFilesRecursive(std::string_view);
            static std::vector<std::string> getDirectories(std::string_view);
            static void copyFile(const std::string&, const std::string&);
            static void deleteDirectory(std::string_view);

            static std::string getFileExtension(std::string_view);
            static std::string getFileName(std::string_view);
            static std::string getFileNameNoExtension(std::string_view);
            static std::string getDirectory(std::string_view);
            static std::string getRelativePath(const std::string&, const std::string&);
            static bool isAbsolutePath(std::string_view);
            static std::string simplifyDirectoryPath(const std::string&);

            static std::string directorySeparator();

        private:
            FileUtil() = default;
            ~FileUtil() = default;

            static std::string pathToString(const std::filesystem::path&);
            static void getFilesRecursive(std::string_view, std::vector<std::string>&);
            static void checkDirectory(std::string_view);
    };

}
