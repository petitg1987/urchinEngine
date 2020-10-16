#include <cassert>

#include "MapUtil.h"
#include "io/StringUtil.h"

namespace urchin {

    //static
    const char MapUtil::DELIMITER = ',';

    std::string MapUtil::serialize(const std::map<std::string, std::string> &map) {
        std::string mapString;
        for (const auto &[key, value] : map) {
            mapString += escape(key) + DELIMITER + escape(value) + DELIMITER;
        }
        if (!map.empty()) {
            mapString.pop_back();
        }

        return mapString;
    }

    std::map<std::string, std::string> MapUtil::deserialize(const std::string &mapString) {
        std::vector<std::string> splitStrings;
        std::vector<std::string> mapListString;
        StringUtil::split(mapString, DELIMITER, splitStrings);

        bool appendNextElement = false;
        for (const auto &elem : splitStrings) {
            if (appendNextElement) {
                mapListString.back() = mapListString.back() + DELIMITER + elem;
            } else {
                mapListString.emplace_back(elem);
            }

            std::string updatedElem = elem;
            StringUtil::replaceAll(updatedElem, "\\\\", "");
            appendNextElement = !updatedElem.empty() && updatedElem.back() == '\\';
        }

        std::map<std::string, std::string> outputMap;
        assert(mapListString.size() % 2 == 0);

        for (std::size_t i=0; i<mapListString.size(); i+=2) {
            std::string key = unescape(mapListString[i]);
            std::string value = unescape(mapListString[i + 1]);
            outputMap.insert(std::pair<std::string, std::string>(key, value));
        }
        return outputMap;
    }

    std::string MapUtil::escape(const std::string &str) {
        std::string escapedStr = str;
        StringUtil::replaceAll(escapedStr, "\\", "\\\\");
        StringUtil::replaceAll(escapedStr, std::string(1, DELIMITER), std::string("\\") + DELIMITER);
        return escapedStr;
    }

    std::string MapUtil::unescape(const std::string &str) {
        std::string unescapedStr = str;
        StringUtil::replaceAll(unescapedStr, std::string("\\") + DELIMITER, std::string(1, DELIMITER));
        StringUtil::replaceAll(unescapedStr, "\\\\", "\\");
        return unescapedStr;
    }

}
