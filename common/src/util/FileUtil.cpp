#include <stdexcept>
#include <filesystem>
#include <sys/stat.h>

#include "FileUtil.h"
#include "util/StringUtil.h"

namespace urchin {

    bool FileUtil::isDirectoryExist(const std::string& directoryPath) {
        struct stat info{};
        if (stat(directoryPath.c_str(), &info) != 0) {
            return false;
        } else if (info.st_mode & (unsigned int)S_IFDIR) {
            return true;
        }
        return false;
    }

    void FileUtil::createDirectory(const std::string& directory) {
        std::error_code errorCode;
        std::filesystem::create_directories(directory, errorCode);
        if(errorCode.value() != 0) {
            throw std::runtime_error("Unable to create the directory: " + directory);
        }
    }

    /**
     * @return File extension. If not extension found: return empty string
     */
    std::string FileUtil::getFileExtension(const std::string& filePath) {
        std::size_t found = filePath.find_last_of('.');
        if (found == std::string::npos) {
            return "";
        }

        return filePath.substr(found+1, filePath.size()-found);

    }

    std::string FileUtil::getFileName(const std::string& filePath) {
        std::size_t found = filePath.find_last_of("/\\");
        if (found == std::string::npos) {
            return filePath;
        }

        return filePath.substr(found+1);
    }

    std::string FileUtil::getFileNameNoExtension(const std::string& filePath) {
        std::string fileName = getFileName(filePath);
        std::string extension = getFileExtension(filePath);

        return fileName.substr(0, fileName.size()-1-extension.size());
    }

    std::string FileUtil::getDirectoryFrom(const std::string& filePath) {
        std::size_t found = filePath.find_last_of("/\\");
        if (found == std::string::npos) {
            return "./";
        }

        return filePath.substr(0, found+1);
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
        for (commonMaxIndex = 0; commonMaxIndex < simplifiedReferenceDirectory.size() && commonMaxIndex<path.size(); ++commonMaxIndex) {
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
            std::size_t found = simplifiedDirectoryPath.find_last_of("/\\", returnDirFound-2);
            if (found == std::string::npos) {
                throw std::domain_error("Invalid directory path: " + directoryPath);
            }

            simplifiedDirectoryPath = simplifiedDirectoryPath.replace(found+1, (returnDirFound+2)-found, "");
            returnDirFound = simplifiedDirectoryPath.find(parentDirectorySymbol);
        }

        StringUtil::replaceAll(simplifiedDirectoryPath, "./", "");

        return simplifiedDirectoryPath;
    }

    void FileUtil::checkDirectory(const std::string& directory) {
        if (directory.empty()) {
            throw std::invalid_argument("Specified directory cannot be null.");
        }

        if (directory.find_last_of("/\\") != directory.size()-1) {
            throw std::invalid_argument("A directory must end by a slash character: " + directory);
        }
    }

}
