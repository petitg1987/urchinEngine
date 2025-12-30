#include <algorithm>
#include <stdexcept>
#include <cctype>

#include "util/StringUtil.h"

namespace urchin {

    void StringUtil::replaceAll(std::string& str, std::string_view from, std::string_view to) {
        std::size_t startPos = 0;
        while ((startPos = str.find(from, startPos)) != std::string::npos) {
            str.replace(startPos, from.length(), to);
            startPos += to.length();
        }
    }

    bool StringUtil::replaceFirst(std::string& str, std::string_view from, std::string_view to) {
        std::size_t pos;
        if ((pos = str.find(from)) != std::string::npos) {
            str.replace(pos, from.length(), to);
            return true;
        }
        return false;
    }

    void StringUtil::replaceLine(std::string& str, std::string_view startLineValue, std::string_view newLineValue) {
        std::size_t startPos;
        if ((startPos = str.find(startLineValue)) != std::string::npos) {
            std::size_t endOfLinePos = str.find('\n', startPos);
            if (endOfLinePos == std::string::npos) [[unlikely]] {
                throw std::runtime_error("Impossible to replace '" + std::string(startLineValue) + "' by '" + std::string(newLineValue) + "' because EOL not found");
            }

            str.replace(startPos, endOfLinePos - startPos, newLineValue);
        }
    }

    void StringUtil::split(std::string_view str, const char& delimiter, std::vector<std::string>& tokens) {
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

    std::vector<std::string> StringUtil::split(std::string_view str, const char& delimiter) {
        std::vector<std::string> tokens;
        split(str, delimiter, tokens);
        return tokens;
    }

    void StringUtil::split(std::string_view str, const std::string& delimiter, std::vector<std::string>& tokens) {
        auto start = str.begin();

        while (start != str.end()) {
            auto temp = std::search(start, str.end(), delimiter.begin(), delimiter.end());
            tokens.emplace_back(start, temp);

            start = temp;
            if (start != str.end()) {
                std::advance(start, delimiter.size());
            }
        }
    }

    std::vector<std::string> StringUtil::split(std::string_view str, const std::string& delimiter) {
        std::vector<std::string> tokens;
        split(str, delimiter, tokens);
        return tokens;
    }

    std::string StringUtil::join(std::span<const std::string> values, const char& delimiter) {
        std::string joinedValues;
        for (unsigned int i = 0; i < values.size(); ++i) {
            joinedValues += values[i];
            if (i + 1 < values.size()) {
                joinedValues += delimiter;
            }
        }
        return joinedValues;
    }

    std::string StringUtil::join(std::span<const std::string> values, const std::string& delimiter) {
        std::string joinedValues;
        for (unsigned int i = 0; i < values.size(); ++i) {
            joinedValues += values[i];
            if (i + 1 < values.size()) {
                joinedValues += delimiter;
            }
        }
        return joinedValues;
    }

    std::size_t StringUtil::countOccurrence(std::string_view str, std::string_view searchStr) {
        std::size_t occurrences = 0;
        std::string::size_type position = 0;
        while ((position = str.find(searchStr, position)) != std::string::npos) {
            ++occurrences;
            position += searchStr.length();
        }
        return occurrences;
    }

    void StringUtil::ltrim(std::string& str) {
        str.erase(str.begin(), std::ranges::find_if(str, [](unsigned char ch) {
            return !std::isspace(ch);
        }));
    }

    void StringUtil::rtrim(std::string& str) {
        str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), str.end());
    }

    void StringUtil::trim(std::string& str) {
        ltrim(str);
        rtrim(str);
    }

    bool StringUtil::insensitiveEquals(std::string_view a, std::string_view b) {
        if (a.size() != b.size()) {
            return false;
        }

        for (std::size_t i = 0; i < a.size(); ++i) {
            if (std::tolower(a[i]) != std::tolower(b[i])) {
                return false;
            }
        }

        return true;
    }

    std::string StringUtil::camelToPascalCase(const std::string& camelCaseStr) {
        std::string pascalCaseStr = camelCaseStr;
        pascalCaseStr[0] = static_cast<char>(std::toupper(pascalCaseStr[0]));
        return pascalCaseStr;
    }

    std::string StringUtil::pascalToCamelCase(const std::string& pascalCaseStr) {
        std::string camelCaseStr = pascalCaseStr;
        camelCaseStr[0] = static_cast<char>(std::tolower(camelCaseStr[0]));
        return camelCaseStr;
    }

    std::string StringUtil::camelToKebabCase(const std::string& camelCaseStr) {
        std::string kebabCaseStr;
        kebabCaseStr.push_back(static_cast<char>(tolower(camelCaseStr[0])));
        for (std::size_t i = 1; i < camelCaseStr.size(); ++i) {
            if (isupper(camelCaseStr[i]) && islower(camelCaseStr[i - 1]) && camelCaseStr[i - 1] != '-') {
                kebabCaseStr += "-";
            }
            kebabCaseStr.push_back(static_cast<char>(tolower(camelCaseStr[i])));
        }
        return kebabCaseStr;
    }

    std::string StringUtil::kebabToCamelCase(const std::string& kebabCaseStr) {
        std::string camelCaseStr;
        camelCaseStr.push_back(static_cast<char>(tolower(kebabCaseStr[0])));
        for (std::size_t i = 1; i < kebabCaseStr.size(); ++i) {
            if (kebabCaseStr[i] == '-' && i + 1 < kebabCaseStr.size() && islower(kebabCaseStr[i + 1])) {
                camelCaseStr.push_back(static_cast<char>(toupper(kebabCaseStr[i + 1])));
                i++;
            } else {
                camelCaseStr.push_back(kebabCaseStr[i]);
            }
        }
        return camelCaseStr;
    }

    char32_t StringUtil::readNextCodepoint(const char*& beginIt, const char* endIt) {
        if (beginIt == endIt) {
            throw std::runtime_error("Nothing to read");
        }

        unsigned char firstByte = static_cast<unsigned char>(*beginIt++);
        if (firstByte < 0x80) { //1 byte UTF-8 (ASCII)
            return firstByte;
        }

        int extraBytes;
        char32_t codepoint = 0;
        if ((firstByte >> 5) == 0b110) { //0b110xxxxx
            extraBytes = 1;
            codepoint = firstByte & 0x1F;
        } else if ((firstByte >> 4) == 0b1110) { //0b1110xxxx
            extraBytes = 2;
            codepoint = firstByte & 0x0F;
        } else if ((firstByte >> 3) == 0b11110) { //0b11110xxx
            extraBytes = 3;
            codepoint = firstByte & 0x07;
        } else {
            throw std::runtime_error("Invalid UTF-8 first byte");
        }

        if (beginIt + extraBytes > endIt) {
            throw std::runtime_error("Invalid UTF-8 sequence");
        }

        for (int i = 0; i < extraBytes; ++i) {
            unsigned char ch = static_cast<unsigned char>(*beginIt++);
            if ((ch >> 6) != 0b10) {
                throw std::runtime_error("UTF-8 continuation bytes must match with 0b10xxxxxx");
            }
            codepoint = (codepoint << 6) | (ch & 0x3F);
        }

        return codepoint;
    }

    /**
     * @param codepoints [out] Output vector of codepoints
     */
    void StringUtil::readCodepoints(const std::string& utf8String, std::vector<char32_t>& codepoints) {
        codepoints.clear();

        if (!utf8String.empty()) {
            codepoints.reserve(utf8String.size()); //estimated memory size
            const char* textIt = &utf8String[0];
            const char* textEndIt = textIt + utf8String.length();
            while (textIt != textEndIt) {
                codepoints.push_back(readNextCodepoint(textIt, textEndIt));
            }
        }
    }

    /**
     * @param utf8String [out] Output UTF-8 string
     */
    void StringUtil::readUtf8String(const std::vector<char32_t>& codepoints, std::string& utf8String) {
        utf8String.clear();

        if (!codepoints.empty()) {
            utf8String.reserve(codepoints.size() * 2); //estimated memory size
            for (char32_t codepoint : codepoints) {
                if (codepoint <= 0x7F) {
                    utf8String += static_cast<char>(codepoint);
                } else if (codepoint <= 0x7FF) {
                    utf8String += static_cast<char>(0xC0 | (codepoint >> 6));
                    utf8String += static_cast<char>(0x80 | (codepoint & 0x3F));
                } else if (codepoint <= 0xFFFF) {
                    utf8String += static_cast<char>(0xE0 | (codepoint >> 12));
                    utf8String += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
                    utf8String += static_cast<char>(0x80 | (codepoint & 0x3F));
                } else {
                    utf8String += static_cast<char>(0xF0 | (codepoint >> 18));
                    utf8String += static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
                    utf8String += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
                    utf8String += static_cast<char>(0x80 | (codepoint & 0x3F));
                }
            }
        }
    }

}
