#include "BacktraceHandler.h"

#include <utility>
#include <iostream>
#include <cassert>
#ifdef _WIN32

#else
    #include <array>
    #include <memory>
    #include <unistd.h>
    #include <dlfcn.h>
    #include <cxxabi.h>
#endif

#include "system/FileHandler.h"
#include "tools/logger/Logger.h"

namespace urchin { //TODO add "-g" symbol for Release

    //static
    std::string BacktraceHandler::programName;
    BacktraceReceiver* BacktraceHandler::backtraceReceiver;

    BacktraceHandler::BacktraceHandler(std::string programName) {
        this->programName = std::move(programName);
    }

    void BacktraceHandler::setup(BacktraceReceiver* backtraceReceiver) {
        this->backtraceReceiver = backtraceReceiver;
        setupSignalHandler();
    }

#ifdef _WIN32
    int BacktraceHandler::addressToLine(void const* addr) {
        std::stringstream cmd;
        cmd << "addr2line -f -p -e " << programName << " " << addr;
        return system(cmd.str().c_str());
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

    LONG WINAPI BacktraceHandler::windowsExceptionHandler(EXCEPTION_POINTERS* exceptionInfo) {
        std::cout << "Caught signal " << exceptionInfo->ExceptionRecord->ExceptionCode << ":" << std::endl;

        if (EXCEPTION_STACK_OVERFLOW != exceptionInfo->ExceptionRecord->ExceptionCode) { //cannot walk the stack in case of stack overflow
            windowsPrintStacktrace(exceptionInfo->ContextRecord);
        } else {
            addressToLine((void*)exceptionInfo->ContextRecord->Rip);
        }
        return EXCEPTION_EXECUTE_HANDLER;
    }

    void Backtrace::setupSignalHandler() {
        SetUnhandledExceptionFilter(windowsExceptionHandler);
    }
#else
    void BacktraceHandler::signalHandler(int sig, siginfo_t* siginfo, void*) {
        std::stringstream ss;
        ss << "Caught signal " << sig << " (code: " << siginfo->si_code << "):" << std::endl;

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
                instructionShiftHex << std::hex << instructionShift - 1;

                ss << "   [bt] " << moduleName << " => " << methodName << " (0x" << instructionShiftHex.str() << ")" << std::endl;

                std::string addr2lineCmd = "addr2line -p -e " + std::string(info.dli_fname) + + " " + instructionShiftHex.str() + " 2> /dev/null";
                std::string cmdResult = exec(addr2lineCmd);
                if(cmdResult.find("??") == std::string::npos) {
                    ss << "        " << cmdResult;
                }
            } else {
                ss << "   [bt] " << symbols[i] << std::endl;
            }
        }
        if (symbols) {
            free(symbols);
        }

        urchin::Logger::instance()->logError(ss.str());
        if(backtraceReceiver) {
            backtraceReceiver->onSignalCaught(sig);
        }

        //re-send the signal to produce a 'core' file
        signal(sig, SIG_DFL);
        kill(getpid(), sig);
    }

    std::string BacktraceHandler::exec(const std::string& cmd) {
        std::array<char, 1024> buffer;
        std::string cmdResult;
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
        if (!pipe) {
            throw std::runtime_error("Function popen() failed for: " + cmd);
        }
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            cmdResult += buffer.data();
        }
        return cmdResult;
    }

    void BacktraceHandler::setupSignalHandler() {
        struct sigaction sigAction = {};
        sigAction.sa_sigaction = signalHandler;
        sigemptyset(&sigAction.sa_mask);
        sigAction.sa_flags = SA_SIGINFO | SA_ONSTACK;

        sigaction(SIGSEGV, &sigAction, nullptr);
        sigaction(SIGFPE, &sigAction, nullptr);
        sigaction(SIGINT, &sigAction, nullptr);
        sigaction(SIGILL, &sigAction, nullptr);
        sigaction(SIGTERM, &sigAction, nullptr);
        sigaction(SIGABRT, &sigAction, nullptr);
    }
#endif

    void BacktraceHandler::simulateError(ErrorSimulationType errorType) {
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
