#pragma once

#include <string>
#include <map>

namespace urchin {

    class MapSerializer {
        public:
            static std::string serialize(const std::map<std::string, std::string, std::less<>>&);
            static std::map<std::string, std::string, std::less<>> deserialize(std::string_view);

        private:
            MapSerializer() = default;
            ~MapSerializer() = default;

            static constexpr char DELIMITER = ',';

            static std::string escape(std::string);
            static std::string unescape(std::string);
    };

}
