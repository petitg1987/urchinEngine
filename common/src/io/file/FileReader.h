#pragma once

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
