#include <sstream>

#include "FileReaderUtil.h"

namespace urchin {
    void FileReaderUtil::nextLine(std::ifstream& file, std::string& buffer) {
        do
        {
            std::getline(file, buffer);

            //delete '\r'
            if (buffer.length() > 0 && buffer[buffer.length() - 1] == '\r') {
                buffer.resize(buffer.length() - 1);
            }
        }while (buffer.length() == 0 && !file.eof());
    }

    std::string FileReaderUtil::readFile(const std::string& filename) {
        std::ifstream fileStream(filename);
        if (fileStream.is_open()) {
            std::stringstream buffer;
            buffer << fileStream.rdbuf();
            return buffer.str();
        }

        throw std::runtime_error("Unable to open file: " + filename);
    }
}
