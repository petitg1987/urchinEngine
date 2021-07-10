#include <sstream>

#include <io/file/FileReader.h>

namespace urchin {

    void FileReader::nextLine(std::ifstream& file, std::string& buffer) {
        do
        {
            std::getline(file, buffer);

            //remove '\r'
            if (buffer.length() > 0 && buffer[buffer.length() - 1] == '\r') {
                buffer.resize(buffer.length() - 1);
            }
        }while (buffer.length() == 0 && !file.eof());
    }

    std::string FileReader::readFile(const std::string& filename) {
        std::ifstream fileStream(filename);
        if (fileStream.is_open()) {
            std::stringstream buffer;
            buffer << fileStream.rdbuf();
            return buffer.str();
        }

        throw std::runtime_error("Unable to open file: " + filename);
    }
}
