#include "util/PathUtil.h"

namespace urchin {

    /**
     * @return Simplified path. Example: xx/yy/../zz/ -> xx/zz/; xx/./ -> xx/
     */
    std::string PathUtil::simplifyDirectoryPath(const std::string& directoryPath) {
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

    /**
     * @param referenceDirectory Directory used as reference for the relative path
     * @param path Path to convert into relative path
     * @return Relative path from the reference directory
     */
    std::string PathUtil::computeRelativePath(const std::string& referenceDirectory, const std::string& path) {
        if (path.empty()) {
            return "";
        }

        if (!FileUtil::isAbsolutePath(referenceDirectory)) {
            throw std::runtime_error("Reference directory must be absolute: " + referenceDirectory);
        } else if (!FileUtil::isAbsolutePath(path)) {
            throw std::runtime_error("Path directory must be absolute: " + path);
        } else if (std::tolower(referenceDirectory[0]) != std::tolower(path[0])) {
            //On Windows, it is not possible to get relative path from a disk (C:\) to another (D:\)
            throw std::runtime_error("Reference directory (" + referenceDirectory + ") and path directory (" + path + ") must be one same drive");
        }

        //remove common directories from path
        std::string simplifiedReferenceDirectory = simplifyDirectoryPath(referenceDirectory);
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

}
