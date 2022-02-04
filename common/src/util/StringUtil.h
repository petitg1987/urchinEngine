#pragma once

#include <string>
#include <vector>
#include <sstream>

namespace urchin {

    class StringUtil {
        public:
            static void replaceAll(std::string&, const std::string&, const std::string&);
            static bool replaceFirst(std::string&, const std::string&, const std::string&);
            static void replaceLine(std::string&, const std::string&, const std::string&);

            static void split(std::string_view, const char&, std::vector<std::string>&);
            static std::vector<std::string> split(std::string_view, const char&);
            static std::string merge(const std::vector<std::string>&, const char&);

            static void ltrim(std::string&);
            static void rtrim(std::string&);
            static void trim(std::string&);

            static bool insensitiveEquals(std::string_view, std::string_view);

            template<class T> static std::string toString(const T&);

        private:
            StringUtil() = default;
            ~StringUtil() = default;
    };

    #include "StringUtil.inl"

}
