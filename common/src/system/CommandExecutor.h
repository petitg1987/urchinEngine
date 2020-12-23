#ifndef URCHINENGINE_COMMANDEXECUTOR_H
#define URCHINENGINE_COMMANDEXECUTOR_H

#include <string>

#include "pattern/singleton/Singleton.h"

namespace urchin {

    class CommandExecutor : public Singleton<CommandExecutor> {
        public:
            friend class Singleton<CommandExecutor>;

            std::string execute(const std::string&) const;

        private:
            CommandExecutor() = default;
            ~CommandExecutor() override = default;
    };

}

#endif
