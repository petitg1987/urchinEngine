#ifndef URCHINENGINE_STRINGUTIL_H
#define URCHINENGINE_STRINGUTIL_H

#include <string>
#include <vector>

#include "pattern/singleton/Singleton.h"

namespace urchin {

    class StringUtil : public Singleton<StringUtil> {
        public:
            friend class Singleton<StringUtil>;

            void replaceAll(std::string&, const std::string&, const std::string&);
            void replaceLine(std::string&, const std::string&, const std::string&);

            void split(const std::string&, const char&, std::vector<std::string>&);

            void ltrim(std::string&);
            void rtrim(std::string&);
            void trim(std::string&);

            bool insensitiveEquals(const std::string&, const std::string&);
            bool endWith(const std::string&, const std::string&);

        private:
            StringUtil() = default;
            ~StringUtil() override = default;
    };

}

#endif
