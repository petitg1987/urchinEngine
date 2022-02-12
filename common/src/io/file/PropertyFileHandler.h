#pragma once

#include <string>
#include <map>

namespace urchin {

    class PropertyFileHandler {
        public:
            explicit PropertyFileHandler(std::string);

            std::map<std::string, std::string, std::less<>> loadPropertyFile() const;
            void savePropertyFile(const std::map<std::string, std::string, std::less<>>&) const;

        private:
            std::string propertiesFilePath;
    };

}
