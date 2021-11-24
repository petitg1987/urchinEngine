#include <algorithm>
#include <stdexcept>

#include <util/StringUtil.h>

namespace urchin {

    void StringUtil::replaceAll(std::string& str, const std::string& from, const std::string& to) {
        size_t startPos = 0;
        while ((startPos = str.find(from, startPos)) != std::string::npos) {
            str.replace(startPos, from.length(), to);
            startPos += to.length();
        }
    }

    void StringUtil::replaceLine(std::string& str, const std::string& startLineValue, const std::string& newLineValue) {
        size_t startPos;
        if ((startPos = str.find(startLineValue)) != std::string::npos) {
            size_t endOfLinePos = str.find('\n', startPos);
            if (endOfLinePos == std::string::npos) {
                throw std::runtime_error("Impossible to replace '" + startLineValue + "' by '" + newLineValue + "' because EOL not found");
            }

            str.replace(startPos, endOfLinePos - startPos, newLineValue);
        }
    }

    void StringUtil::split(const std::string& str, const char& delimiter, std::vector<std::string>& tokens) {
        auto start = str.begin();

        while (start != str.end()) {
            auto temp = std::find(start, str.end(), delimiter);
            tokens.emplace_back(start, temp);

            start = temp;
            if (start != str.end()) {
                std::advance(start, 1);
            }
        }
    }

    std::vector<std::string> StringUtil::split(const std::string& str, const char& delimiter) {
        std::vector<std::string> tokens;
        split(str, delimiter, tokens);
        return tokens;
    }

    std::string StringUtil::merge(const std::vector<std::string>& values, const char& delimiter) {
        std::string mergedValues;
        for (unsigned int i = 0; i < values.size(); ++i) {
            mergedValues += values[i];
            if (i + 1 < values.size()) {
                mergedValues += delimiter;
            }
        }
        return mergedValues;
    }

    void StringUtil::ltrim(std::string& str) {
        str.erase(str.begin(), std::ranges::find_if(str, std::not1(std::ptr_fun<int, int>(std::isspace))));
    }

    void StringUtil::rtrim(std::string& str) {
        str.erase(std::find_if(str.rbegin(), str.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), str.end());
    }

    void StringUtil::trim(std::string& str) {
        ltrim(str);
        rtrim(str);
    }

    bool StringUtil::insensitiveEquals(const std::string& a, const std::string& b) {
        if (a.size() != b.size()) {
            return false;
        }

        for (std::size_t i = 0; i < a.size(); ++i) {
            if (tolower(a[i]) != tolower(b[i])) {
                return false;
            }
        }

        return true;
    }

    bool StringUtil::startWith(const std::string& str, const std::string& startStr) {
        return str.rfind(startStr, 0) == 0;
    }

    bool StringUtil::endWith(const std::string& str, const std::string& endStr) {
        if (endStr.size() > str.size()) {
            return false;
        }
        return std::equal(endStr.rbegin(), endStr.rend(), str.rbegin());
    }

}
