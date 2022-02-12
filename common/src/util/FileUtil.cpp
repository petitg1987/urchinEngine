#include <stdexcept>
#include <filesystem>
#include <fstream>
#include <sys/stat.h>

#include <util/FileUtil.h>
#include <util/StringUtil.h>
#include <util/UserAuthorityException.h>

namespace urchin {

    bool FileUtil::isDirectoryExist(const std::string& directoryPath) {
        struct stat info{};
        if (stat(directoryPath.c_str(), &info) != 0) {
            return false;
        }
        return info.st_mode & (unsigned int)S_IFDIR;
    }

    bool FileUtil::isFileExist(const std::string& filePath) {
        struct stat info{};
        if (stat(filePath.c_str(), &info) != 0) {
            return false;
        }
        return info.st_mode & (unsigned int)S_IFREG;
    }

    /**
     * @return Return size of the file in byte
     */
    long FileUtil::getFileSize(const std::string& filePath) {
        struct stat info{};
        if (stat(filePath.c_str(), &info) != 0) {
            return -1;
        }
        return info.st_size;
    }

    void FileUtil::createDirectory(std::string_view directory) {
        std::error_code errorCode;
        std::filesystem::create_directories(directory, errorCode);
        if (errorCode.value() != 0) {
            throw UserAuthorityException("Unable to create the directory: " + std::string(directory), "Check that the application has enough right to create the directory: " + std::string(directory));
        }
    }

    void FileUtil::createEmptyFile(const std::string& filename) {
        std::ofstream output(filename);
        if (!output.is_open()) {
            throw std::runtime_error("Unable to create empty file: " + filename);
        }
    }

    void FileUtil::copyDirectoryContent(std::string_view srcDirectory, const std::string& dstDirectory) {
        //Similar to "std::filesystem::copy(src, dst, std::filesystem::copy_options::skip_existing);" method
        //Unfortunately, the "copy" method doesn't work correctly on MSYS2: https://github.com/msys2/MSYS2-packages/issues/1937

        checkDirectory(dstDirectory);
        for (const auto& entry : std::filesystem::directory_iterator(srcDirectory)) {
            if (entry.is_regular_file()) {
                std::string srcFile = entry.path().string();
                std::string dstFile = dstDirectory + std::string(FileUtil::getFileName(srcFile));

                if (!isFileExist(dstFile)) {
                    copyFile(srcFile, dstFile);
                }
            }
        }
    }

    std::vector<std::string> FileUtil::getFilesRecursive(const std::string& directory) {
        checkDirectory(directory);

        std::vector<std::string> filenames;
        getFilesRecursive(directory, filenames);
        return filenames;
    }

    void FileUtil::getFilesRecursive(std::string_view directory, std::vector<std::string>& filenames) {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                filenames.push_back(entry.path().string());
            } else if (entry.is_directory()) {
                getFilesRecursive(entry.path().string(), filenames);
            }
        }
    }

    void FileUtil::copyFile(const std::string& srcFile, const std::string& dstFile) {
        std::ifstream src(srcFile, std::ios::binary);
        if (!src.is_open()) {
            throw std::runtime_error("Unable to open file: " + srcFile);
        }

        std::ofstream dst(dstFile, std::ios::binary);
        if (!dst.is_open()) {
            throw UserAuthorityException("Unable to open file: " + dstFile, "Check that the application has enough right to create the file: " + dstFile);
        }
        dst << src.rdbuf();
    }

    void FileUtil::deleteDirectory(std::string_view directory) {
        checkDirectory(directory);
        std::filesystem::remove_all(directory);
    }

    /**
     * @return File extension. If not extension found: return empty string
     */
    std::string_view FileUtil::getFileExtension(std::string_view filePath) {
        std::size_t extensionPos = filePath.find_last_of('.');
        if (extensionPos == std::string::npos) {
            return "";
        }

        return filePath.substr(extensionPos + 1, filePath.size() - extensionPos);
    }

    std::string_view FileUtil::getFileName(std::string_view filePath) {
        std::size_t fileNamePos = filePath.find_last_of("/\\");
        if (fileNamePos == std::string::npos) {
            return filePath;
        }

        return filePath.substr(fileNamePos + 1);
    }

    std::string_view FileUtil::getFileNameNoExtension(std::string_view filePath) {
        std::string_view fileName = getFileName(filePath);
        std::string_view extension = getFileExtension(filePath);

        return fileName.substr(0, fileName.size() - 1 - extension.size());
    }

    std::string_view FileUtil::getDirectory(std::string_view filePath) {
        std::size_t found = filePath.find_last_of("/\\");
        if (found == std::string::npos) {
            return "./";
        }

        return filePath.substr(0, found + 1);
    }

    /**
     * @param referenceDirectory Directory used as reference for the relative path
     * @param path Path to convert into relative path
     * @return Relative path from the reference directory
     */
    std::string FileUtil::getRelativePath(const std::string& referenceDirectory, const std::string& path) {
        checkDirectory(referenceDirectory);
        std::string simplifiedReferenceDirectory = simplifyDirectoryPath(referenceDirectory);

        //remove common directories from path
        unsigned int commonMaxIndex;
        for (commonMaxIndex = 0; commonMaxIndex < simplifiedReferenceDirectory.size() && commonMaxIndex < path.size(); ++commonMaxIndex) {
            if (simplifiedReferenceDirectory[commonMaxIndex] != path[commonMaxIndex]) {
                break;
            }
        }
        std::string relativePath = path.substr(commonMaxIndex);

        //add '../' to relative path
        for (std::size_t i = commonMaxIndex; i < simplifiedReferenceDirectory.size(); ++i) {
            if (simplifiedReferenceDirectory[i] == '/' || simplifiedReferenceDirectory[i] == '\\') {
                relativePath = std::string("../").append(relativePath);
            }
        }

        return relativePath;
    }

    /**
     * @return Simplified path. Example: xx/yy/../zz/ -> xx/zz/; xx/./ -> xx/
     */
    std::string FileUtil::simplifyDirectoryPath(const std::string& directoryPath) {
        checkDirectory(directoryPath);
        const std::string parentDirectorySymbol = "..";

        std::string simplifiedDirectoryPath = directoryPath;
        std::size_t returnDirFound = simplifiedDirectoryPath.find(parentDirectorySymbol);
        while (returnDirFound != std::string::npos) {
            std::size_t found = simplifiedDirectoryPath.find_last_of("/\\", returnDirFound - 2);
            if (found == std::string::npos) {
                throw std::domain_error("Invalid directory path: " + directoryPath);
            }

            simplifiedDirectoryPath = simplifiedDirectoryPath.replace(found + 1, (returnDirFound + 2) - found, "");
            returnDirFound = simplifiedDirectoryPath.find(parentDirectorySymbol);
        }

        StringUtil::replaceAll(simplifiedDirectoryPath, "./", "");

        return simplifiedDirectoryPath;
    }

    void FileUtil::checkDirectory(std::string_view directory) {
        if (directory.empty()) {
            throw std::invalid_argument("Specified directory cannot be null.");
        }

        if (directory.find_last_of("/\\") != directory.size() - 1) {
            throw std::invalid_argument("A directory must end by a slash character: " + std::string(directory));
        }
    }

}
