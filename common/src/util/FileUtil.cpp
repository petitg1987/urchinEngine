#include <stdexcept>
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
                std::string srcFile = pathToString(entry.path());
                std::string dstFile = dstDirectory + FileUtil::getFileName(srcFile);

                if (!isFileExist(dstFile)) {
                    copyFile(srcFile, dstFile);
                }
            }
        }
    }

    std::string FileUtil::pathToString(const std::filesystem::path& path) {
        #ifdef _WIN32
            //NTFS filesystem use UTF-16 encoding. Therefore, without this specific code, the application won't work correctly if it is stored in a folder with characters others than ASCII (e.g.: é).
            auto pathU16String = path.u16string();
            return std::string(pathU16String.begin(), pathU8String.end());
        #else
            auto pathU8String = path.u8string();
            return std::string(pathU8String.begin(), pathU8String.end());
        #endif
    }

    std::vector<std::string> FileUtil::getFiles(std::string_view directory) {
        checkDirectory(directory);

        std::vector<std::string> filenames;
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                filenames.push_back(pathToString(entry.path()));
            }
        }
        return filenames;
    }

    std::vector<std::string> FileUtil::getFilesRecursive(std::string_view directory) {
        checkDirectory(directory);

        std::vector<std::string> filenames;
        getFilesRecursive(directory, filenames);
        return filenames;
    }

    void FileUtil::getFilesRecursive(std::string_view directory, std::vector<std::string>& filenames) {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                filenames.push_back(pathToString(entry.path()));
            } else if (entry.is_directory()) {
                getFilesRecursive(pathToString(entry.path()), filenames);
            }
        }
    }

    std::vector<std::string> FileUtil::getDirectories(std::string_view directory) {
        checkDirectory(directory);

        std::vector<std::string> directories;
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_directory()) {
                directories.push_back(pathToString(entry.path()));
            }
        }
        return directories;
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
    std::string FileUtil::getFileExtension(std::string_view filePath) {
        std::size_t extensionPos = filePath.find_last_of('.');
        if (extensionPos == std::string::npos) {
            return "";
        }

        return std::string(filePath.substr(extensionPos + 1));
    }

    std::string FileUtil::getFileName(std::string_view filePath) {
        std::size_t fileNamePos = filePath.find_last_of("/\\");
        if (fileNamePos == std::string::npos) {
            return std::string(filePath);
        }

        return std::string(filePath.substr(fileNamePos + 1));
    }

    std::string FileUtil::getFileNameNoExtension(std::string_view filePath) {
        std::string fileName = getFileName(filePath);
        std::string extension = getFileExtension(filePath);

        return std::string(fileName.substr(0, fileName.size() - 1 - extension.size()));
    }

    std::string FileUtil::getDirectory(std::string_view filePath) {
        std::size_t found = filePath.find_last_of("/\\");
        if (found == std::string::npos) {
            return "./";
        }

        return std::string(filePath.substr(0, found + 1));
    }

    /**
     * @param referenceDirectory Directory used as reference for the relative path
     * @param path Path to convert into relative path
     * @return Relative path from the reference directory
     */
    std::string FileUtil::getRelativePath(const std::string& referenceDirectory, const std::string& path) {
        checkDirectory(referenceDirectory);
        if (!FileUtil::isAbsolutePath(referenceDirectory)) {
            throw std::runtime_error("Reference directory must be absolute: " + referenceDirectory);
        } else if (!FileUtil::isAbsolutePath(path)) {
            throw std::runtime_error("Path directory must be absolute: " + path);
        } else if (std::tolower(referenceDirectory[0]) != std::tolower(path[0])) {
            //On Windows, it is not possible to get relative path from a disk (C:\) to another (D:\)
            throw std::runtime_error("Reference directory (" + referenceDirectory + ") and path directory (" + path + ") must be one same drive");
        }
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

    bool FileUtil::isAbsolutePath(std::string_view path) {
        #ifdef _WIN32
            return path.size() >= 2 && path[1] == ':' && std::isalpha(path[0]);
        #else
            return !path.empty() && path[0] == '/';
        #endif
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

    std::string FileUtil::directorySeparator() {
        #ifdef _WIN32
            return "\\";
        #else
            return "/";
        #endif
    }

}
