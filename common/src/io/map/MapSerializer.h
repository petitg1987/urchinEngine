#ifndef URCHINENGINE_MAPSERIALIZER_H
#define URCHINENGINE_MAPSERIALIZER_H

#include <string>
#include <map>

#include "pattern/singleton/Singleton.h"

namespace urchin {

    class MapSerializer : public Singleton<MapSerializer> {
        public:
            friend class Singleton<MapSerializer>;

            std::string serialize(const std::map<std::string, std::string>&);
            std::map<std::string, std::string> deserialize(const std::string&);

        private:
            MapSerializer() = default;
            ~MapSerializer() override = default;

            static const char DELIMITER;

            std::string escape(const std::string&);
            std::string unescape(const std::string&);
    };

}

#endif
