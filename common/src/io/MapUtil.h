#ifndef URCHINENGINE_MAPUTIL_H
#define URCHINENGINE_MAPUTIL_H

#include <string>
#include <map>

namespace urchin {

    class MapUtil {
        public:
            static std::string serialize(const std::map<std::string, std::string> &);
            static std::map<std::string, std::string> deserialize(const std::string &);

        private:
            MapUtil() = default;
            ~MapUtil() = default;

            static const char DELIMITER;

            static std::string escape(const std::string &);
            static std::string unescape(const std::string &str);
    };

}

#endif
