#pragma once

#include <string>
#include <vector>
#include <sstream>

namespace urchin {

    class StringUtil {
        public:
            static void replaceAll(std::string&, std::string_view, std::string_view);
            static bool replaceFirst(std::string&, std::string_view, std::string_view);
            static void replaceLine(std::string&, std::string_view, std::string_view);

            static void split(std::string_view, const char&, std::vector<std::string>&);
            static std::vector<std::string> split(std::string_view, const char&);
            static std::string merge(const std::vector<std::string>&, const char&);
            static std::string merge(const std::vector<std::string>&, const std::string&);
            static std::size_t countOccurrence(std::string_view, std::string_view);

            static void ltrim(std::string&);
            static void rtrim(std::string&);
            static void trim(std::string&);

            static bool insensitiveEquals(std::string_view, std::string_view);

            [[nodiscard]] static std::string camelToPascalCase(const std::string&);
            [[nodiscard]] static std::string pascalToCamelCase(const std::string&);
            [[nodiscard]] static std::string camelToKebabCase(const std::string&);
            [[nodiscard]] static std::string kebabToCamelCase(const std::string&);

            template<class T> static std::string toString(const T&);

        private:
            StringUtil() = default;
            ~StringUtil() = default;
    };

    #include "StringUtil.inl"

}
