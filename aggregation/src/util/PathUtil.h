#pragma once

#include <string>

namespace urchin {

    class PathUtil {
        public:
            static std::string simplifyDirectoryPath(const std::string&);
            static std::string computeRelativePath(const std::string&, const std::string&);

        private:
            PathUtil() = default;
            ~PathUtil() = default;
    };

}
