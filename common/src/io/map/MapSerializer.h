#ifndef URCHINENGINE_MAPSERIALIZER_H
#define URCHINENGINE_MAPSERIALIZER_H

#include <string>
#include <map>

namespace urchin {

    class MapSerializer {
        public:
            static std::string serialize(const std::map<std::string, std::string>&);
            static std::map<std::string, std::string> deserialize(const std::string&);

        private:
            MapSerializer() = default;
            ~MapSerializer() = default;

            static const char DELIMITER;

            static std::string escape(const std::string&);
            static std::string unescape(const std::string&);
    };

}

#endif
