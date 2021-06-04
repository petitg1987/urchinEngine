#pragma once

#include <string>

namespace urchin {

    class CommandExecutor {
        public:
            CommandExecutor() = delete;

            static std::string execute(const std::string&);
    };

}
