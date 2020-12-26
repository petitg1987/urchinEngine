#ifndef URCHINENGINE_FILEREADER_H
#define URCHINENGINE_FILEREADER_H

#include <string>
#include <fstream>

#include "pattern/singleton/Singleton.h"

namespace urchin {

    class FileReader : public Singleton<FileReader> {
        public:
            friend class Singleton<FileReader>;

            void nextLine(std::ifstream&, std::string&);
            std::string readFile(const std::string&);

        private:
            FileReader() = default;
            ~FileReader() override = default;
    };

}

#endif
