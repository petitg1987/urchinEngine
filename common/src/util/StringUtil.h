#pragma once

#include <sstream>
#include <string>
#include <vector>
#include <ranges>
#include <span>

namespace urchin {

    class StringUtil {
        public:
            static void replaceAll(std::string&, std::string_view, std::string_view);
            static bool replaceFirst(std::string&, std::string_view, std::string_view);
            static void replaceLine(std::string&, std::string_view, std::string_view);

            static void split(std::string_view, const char&, std::vector<std::string>&);
            static std::vector<std::string> split(std::string_view, const char&);
            static void split(std::string_view, const std::string&, std::vector<std::string>&);
            static std::vector<std::string> split(std::string_view, const std::string&);
            static std::string join(std::span<const std::string>, const char&);
            static std::string join(std::span<const std::string>, const std::string&);
            static std::size_t countOccurrence(std::string_view, std::string_view);

            static void ltrim(std::string&);
            static void rtrim(std::string&);
            static void trim(std::string&);

            static bool insensitiveEquals(std::string_view, std::string_view);

            [[nodiscard]] static std::string camelToPascalCase(const std::string&);
            [[nodiscard]] static std::string pascalToCamelCase(const std::string&);
            [[nodiscard]] static std::string camelToKebabCase(const std::string&);
            [[nodiscard]] static std::string kebabToCamelCase(const std::string&);

            [[nodiscard]] static char32_t readNextCodepoint(const char*&, const char*);
            static void readCodepoints(const std::string&, std::vector<char32_t>&);
            static std::string readUtf8String(std::span<const char32_t>);

            template<class T> static std::string toString(const T&);

        private:
            StringUtil() = default;
            ~StringUtil() = default;
    };

    #include "StringUtil.inl"

}
