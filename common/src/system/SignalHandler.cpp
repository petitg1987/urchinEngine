#include "SignalHandler.h"

#include <iostream>
#include <cassert>
#ifdef _WIN32

#else
    #include <array>
    #include <unistd.h>
    #include <dlfcn.h>
#endif

#include "system/FileHandler.h"
#include "tools/logger/Logger.h"
#include "io/StringUtil.h"

namespace urchin {

    SignalHandler::SignalHandler() {
        setupSignalHandler();
    }

    void SignalHandler::registerSignalReceptor(std::unique_ptr<SignalReceptor>&& signalReceptor) {
        this->signalReceptor = std::move(signalReceptor);

    }

#ifdef _WIN32
    void Backtrace::setupSignalHandler() {
        SetUnhandledExceptionFilter(windowsExceptionHandler);
    }

    LONG WINAPI BacktraceHandler::windowsExceptionHandler(EXCEPTION_POINTERS* exceptionInfo) {
        std::stringstream ss;
        ss << "Caught signal " << exceptionInfo->ExceptionRecord->ExceptionCode << ":" << std::endl;

        if (EXCEPTION_STACK_OVERFLOW != exceptionInfo->ExceptionRecord->ExceptionCode) { //cannot walk the stack in case of stack overflow
            instance()->windowsPrintStacktrace(exceptionInfo->ContextRecord);
        } else {
            instance()->addressToLine((void*)exceptionInfo->ContextRecord->Rip);
        }
        return EXCEPTION_EXECUTE_HANDLER;
    }

    void BacktraceHandler::windowsPrintStacktrace(CONTEXT* context) {
        SymInitialize(GetCurrentProcess(), 0, true);
        STACKFRAME frame = {0};
        frame.AddrPC.Offset = context->Rip;
        frame.AddrPC.Mode = AddrModeFlat;
        frame.AddrStack.Offset = context->Rsp;
        frame.AddrStack.Mode = AddrModeFlat;
        frame.AddrFrame.Offset = context->Rbp;
        frame.AddrFrame.Mode = AddrModeFlat;

        while (StackWalk(IMAGE_FILE_MACHINE_I386, GetCurrentProcess(), GetCurrentThread(), &frame, context, 0, SymFunctionTableAccess, SymGetModuleBase, 0)) {
            addr2line((void*)frame.AddrPC.Offset);
        }

        SymCleanup(GetCurrentProcess());
    }

    int BacktraceHandler::addressToLine(void const* addr) {
        std::stringstream cmd;
        cmd << "addr2line -f -p -e " << programName << " " << addr;
        return system(cmd.str().c_str());
    }

#else

    void SignalHandler::setupSignalHandler() {
        struct sigaction sigAction = {};
        sigAction.sa_sigaction = &signalHandler;
        sigemptyset(&sigAction.sa_mask);
        sigAction.sa_flags = SA_SIGINFO | SA_ONSTACK;

        sigaction(SIGSEGV, &sigAction, nullptr);
        sigaction(SIGFPE, &sigAction, nullptr);
        sigaction(SIGINT, &sigAction, nullptr);
        sigaction(SIGILL, &sigAction, nullptr);
        sigaction(SIGTERM, &sigAction, nullptr);
        sigaction(SIGABRT, &sigAction, nullptr);
    }

    void SignalHandler::signalHandler(int signalId, siginfo_t* signalInfo, void*) {
        std::stringstream ss;
        ss << "Caught signal " << signalId << " (code: " << signalInfo->si_code << "):" << std::endl;

        constexpr int maxStackFrames = 256;
        void *traces[maxStackFrames];
        int traceSize = backtrace(traces, maxStackFrames);
        char **symbols = backtrace_symbols(traces, traceSize);

        for (int i = 1; i < traceSize; ++i) {
            Dl_info info;
            if(dladdr(traces[i], &info) && info.dli_sname) {
                std::string moduleName = info.dli_fname != nullptr ? FileHandler::getFileName(info.dli_fname) : "[no module]";
                int status;
                char* demangledMethod = abi::__cxa_demangle(info.dli_sname, nullptr, 0, &status);
                std::string methodName = status == 0 ? demangledMethod : symbols[i];

                long methodShift = (char *)info.dli_saddr - (char *)info.dli_fbase;
                long methodInstructionShift = (char *)traces[i] - (char *)info.dli_saddr;
                long instructionShift = methodShift + methodInstructionShift - 1 ; //see https://stackoverflow.com/a/63841497
                std::stringstream instructionShiftHex;
                instructionShiftHex << std::hex << instructionShift;

                ss << "\t[bt] [" << moduleName << "] " << methodName << " (0x" << instructionShiftHex.str() << ")";

                std::string addr2lineCmd = "addr2line -p -e " + std::string(info.dli_fname) + + " " + instructionShiftHex.str() + " 2> /dev/null";
                std::string cmdResult = instance()->executeCommand(addr2lineCmd);
                if(cmdResult.find("??") == std::string::npos) {
                    ss << std::endl << "\t[bt]\t=> " << cmdResult;
                }
            } else {
                ss << "\t[bt] " << symbols[i];
            }

            if(i < traceSize - 1) {
                ss << std::endl;
            }
        }

        if (symbols) {
            free(symbols);
        }

        urchin::Logger::instance()->logError(ss.str());
        if(instance()->signalReceptor) {
            instance()->signalReceptor->onSignalReceived(signalId);
        }

        //re-send the signal to produce a 'core' file
        signal(signalId, SIG_DFL);
        kill(getpid(), signalId);
    }

    std::string SignalHandler::executeCommand(const std::string& cmd) {
        std::array<char, 1024> buffer;
        std::string cmdResult;
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
        if (!pipe) {
            throw std::runtime_error("Function popen() failed for: " + cmd);
        }

        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            cmdResult += buffer.data();
        }

        if(StringUtil::endWith(cmdResult, "\n")) {
            return cmdResult.substr(0, cmdResult.size() - 1);
        }
        return cmdResult;
    }
#endif

    void SignalHandler::simulateError(ErrorSimulationType errorType) {
        if (SEGMENTATION_FAULT == errorType) {
            int* p = (int*)0x999999999;
            *p = 0;
        } else if (DIVIDE_BY_ZERO == errorType) {
            int zero = 0;
            int division = 1 / zero;
            std::cerr << division;
        } else if(EXCEPTION == errorType) {
            throw std::runtime_error("BacktraceHandler::simulateError");
        } else if(ASSERT == errorType) {
            assert(false);
        } else if (ILLEGAL_INSTRUCTION == errorType) {
            raise(SIGILL);
        } else {
            throw std::runtime_error("Unknown error simulation type: " + std::to_string(errorType));
        }
    }

}
