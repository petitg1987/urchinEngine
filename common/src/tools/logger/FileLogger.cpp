#include <fstream>
#include <stdexcept>
#include <vector>
#include <ctime>

#include "tools/logger/FileLogger.h"
#include "system/FileHandler.h"

namespace urchin {

    FileLogger::FileLogger(std::string filename) :
            Logger(),
            filename(std::move(filename)) {

    }

    const std::string& FileLogger::getFilename() const {
        return filename;
    }

    std::string FileLogger::retrieveContent(unsigned long maxSize) const {
        std::ifstream ifs(filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
        if (!ifs.is_open()) {
            throw std::runtime_error("Unable to open file: " + filename);
        }
        auto fileSize = ifs.tellg();
        auto readSize = std::min((unsigned long)fileSize, maxSize);
        ifs.seekg(-(int)readSize, std::ios::end);

        std::vector<char> bytes(readSize);
        ifs.read(bytes.data(), static_cast<std::streamsize>(readSize));

        return std::string(bytes.data(), readSize);
    }

    void FileLogger::purge() const {
        std::ofstream file;
        file.open(filename, std::ofstream::out | std::ofstream::trunc);
        if (!file.is_open()) {
            throw std::runtime_error("Unable to open file: " + filename);
        }
        file.close();
    }

    void FileLogger::archive() const {
        std::string epoch = std::to_string(std::time(nullptr));
        std::string extension = FileHandler::getFileExtension(filename);
        std::string archiveFilename = filename.substr(0, filename.size()-extension.size()-1) + "_" + epoch + "." + extension;

        std::ofstream archiveStream(archiveFilename);
        if (!archiveStream.is_open()) {
            throw std::runtime_error("Unable to open file: " + filename);
        }
        archiveStream << retrieveContent(std::numeric_limits<unsigned long>::max()) << std::endl;
        archiveStream.close();

        purge();
    }

    void FileLogger::write(const std::string& msg) {
        std::ofstream file;
        file.open(filename, std::ios::app);
        if (file.fail()) {
            throw std::runtime_error("Unable to open file: " + filename);
        }
        file.write(msg.c_str(), static_cast<std::streamsize>(msg.length()));
        file.flush();
        file.close();
    }

}
