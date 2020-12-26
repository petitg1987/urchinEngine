#ifndef URCHINENGINE_COMMANDEXECUTOR_H
#define URCHINENGINE_COMMANDEXECUTOR_H

#include <string>

namespace urchin {

    class CommandExecutor {
        public:
            CommandExecutor() = delete;

            static std::string execute(const std::string&);
    };

}

#endif
