#include <array>
#include <memory>
#include <stdexcept>

#include <system/CommandExecutor.h>

namespace urchin {

    std::string CommandExecutor::execute(const std::string& cmd) {
        std::array<char, 1024> buffer{};
        std::string cmdResult;
        std::unique_ptr<FILE, int(*)(FILE*)> pipe(popen(cmd.c_str(), "r"), &pclose);
        if (!pipe) {
            throw std::runtime_error("Function popen() failed for: " + cmd);
        }

        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            cmdResult.append(buffer.data());
        }

        if (cmdResult.ends_with("\n")) {
            return cmdResult.substr(0, cmdResult.size() - 1);
        }
        return cmdResult;
    }

}
