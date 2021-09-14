#pragma once

#include <string>
#include <vector>

namespace urchin {

    class StringUtil {
        public:
            static void replaceAll(std::string&, const std::string&, const std::string&);
            static void replaceLine(std::string&, const std::string&, const std::string&);

            static void split(const std::string&, const char&, std::vector<std::string>&);
            static std::vector<std::string> split(const std::string&, const char&);
            static std::string merge(const std::vector<std::string>&, const char&);

            static void ltrim(std::string&);
            static void rtrim(std::string&);
            static void trim(std::string&);

            static bool insensitiveEquals(const std::string&, const std::string&);
            static bool startWith(const std::string&, const std::string&);
            static bool endWith(const std::string&, const std::string&);

        private:
            StringUtil() = default;
            ~StringUtil() = default;
    };

}
