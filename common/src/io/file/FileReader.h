#ifndef URCHINENGINE_FILEREADER_H
#define URCHINENGINE_FILEREADER_H

#include <string>
#include <fstream>

namespace urchin {

    class FileReader {
        public:
            static void nextLine(std::ifstream&, std::string&);
            static std::string readFile(const std::string&);

        private:
            FileReader() = default;
            ~FileReader() = default;
    };

}

#endif
