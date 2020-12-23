#ifndef LASERRIDDLE_COMMANDEXECUTOR_H
#define LASERRIDDLE_COMMANDEXECUTOR_H

#include <string>

namespace urchin {

    class CommandExecutor {
        public:
            std::string execute(const std::string&) const;
    };

}

#endif
