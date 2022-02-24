#include <cassert>

#include <io/map/MapSerializer.h>
#include <util/StringUtil.h>

namespace urchin {

    std::string MapSerializer::serialize(const std::map<std::string, std::string, std::less<>>& map) {
        std::string mapString;
        for (const auto& [key, value] : map) {
            mapString += escape(key) + DELIMITER + escape(value) + DELIMITER;
        }
        if (!map.empty()) {
            mapString.pop_back();
        }

        return mapString;
    }

    std::map<std::string, std::string, std::less<>> MapSerializer::deserialize(std::string_view mapString) {
        std::vector<std::string> mapListString;
        std::vector<std::string> splitStrings = StringUtil::split(mapString, DELIMITER);

        bool appendNextElement = false;
        for (const auto& elem : splitStrings) {
            if (appendNextElement) {
                mapListString.back() = mapListString.back() + DELIMITER + elem;
            } else {
                mapListString.emplace_back(elem);
            }

            std::string updatedElem = elem;
            StringUtil::replaceAll(updatedElem, "\\\\", "");
            appendNextElement = !updatedElem.empty() && updatedElem.back() == '\\';
        }

        std::map<std::string, std::string, std::less<>> outputMap;
        assert(mapListString.size() % 2 == 0);

        for (std::size_t i = 0; i < mapListString.size(); i += 2) {
            std::string key = unescape(mapListString[i]);
            std::string value = unescape(mapListString[i + 1]);
            outputMap.try_emplace(key, value);
        }
        return outputMap;
    }

    std::string MapSerializer::escape(std::string str) {
        std::string escapedStr = std::move(str);
        StringUtil::replaceAll(escapedStr, "\\", "\\\\");
        StringUtil::replaceAll(escapedStr, std::string(1, DELIMITER), std::string("\\") + DELIMITER);
        return escapedStr;
    }

    std::string MapSerializer::unescape(std::string str) {
        std::string unescapedStr = std::move(str);
        StringUtil::replaceAll(unescapedStr, std::string("\\") + DELIMITER, std::string(1, DELIMITER));
        StringUtil::replaceAll(unescapedStr, "\\\\", "\\");
        return unescapedStr;
    }

}
