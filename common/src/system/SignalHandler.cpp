#include "SignalHandler.h"

#include <iostream>
#include <cassert>
#ifdef _WIN32
    #include <windows.h>
    #include <imagehlp.h>
#else
    #include <execinfo.h>
    #include <cxxabi.h>
    #include <array>
    #include <unistd.h>
    #include <dlfcn.h>
#endif

#include "system/FileHandler.h"
#include "tools/logger/Logger.h"
#include "io/StringUtil.h"

namespace urchin {

    SignalHandler::SignalHandler() :
            isInitialized(false) {

    }

    void SignalHandler::initialize() {
        if(!isInitialized) {
            setupSignalHandler();
            isInitialized = true;
        }
    }

    void SignalHandler::registerSignalReceptor(const std::shared_ptr<SignalReceptor>& signalReceptor) {
        initialize();

        this->signalReceptor = std::move(signalReceptor);
    }

#ifdef _WIN32
    void SignalHandler::setupSignalHandler() {
        SetUnhandledExceptionFilter(signalHandler);
    }

    long __stdcall SignalHandler::signalHandler(EXCEPTION_POINTERS* exceptionInfo) {
        std::stringstream ss;
        ss << "Caught signal " << exceptionInfo->ExceptionRecord->ExceptionCode << ":" << std::endl;

        HANDLE process = GetCurrentProcess();
        HANDLE thread = GetCurrentThread();
        CONTEXT* context = exceptionInfo->ContextRecord;

        SymInitialize(process, nullptr, true);
        STACKFRAME frame = {};
        frame.AddrPC.Offset = context->Rip;
        frame.AddrPC.Mode = AddrModeFlat;
        frame.AddrStack.Offset = context->Rsp;
        frame.AddrStack.Mode = AddrModeFlat;
        frame.AddrFrame.Offset = context->Rbp;
        frame.AddrFrame.Mode = AddrModeFlat;

        unsigned int stackCount = 0;
        while (StackWalk(IMAGE_FILE_MACHINE_AMD64, process, thread, &frame, context, nullptr, SymFunctionTableAccess, SymGetModuleBase, nullptr)) {
            std::string moduleName;
            DWORD64 moduleBase = SymGetModuleBase(process, frame.AddrPC.Offset);
            char moduleBuffer[MAX_PATH];
            if (moduleBase && GetModuleFileNameA((HINSTANCE)moduleBase, moduleBuffer, MAX_PATH)) {
                moduleName = FileHandler::getFileName(moduleBuffer);
            }

            std::string methodName = "{method not found}";
            DWORD64 offset = 0;
            char symbolBuffer[sizeof(IMAGEHLP_SYMBOL) + 255];
            auto symbol = (PIMAGEHLP_SYMBOL)symbolBuffer;
            symbol->SizeOfStruct = (sizeof(IMAGEHLP_SYMBOL)) + 255;
            symbol->MaxNameLength = 254;
            if (SymGetSymFromAddr(process, frame.AddrPC.Offset, &offset, symbol)) {
                methodName = symbol->Name;
            }

            void* instructionShift = (char*)frame.AddrPC.Offset - 1; //see https://stackoverflow.com/a/63841497

            ss << "\t[bt] [" << moduleName << "] " << methodName << " (addr2line -f -e " << moduleName << " " << instructionShift << ")" << std::endl;

            if (++stackCount > 250) { //avoid infinite loop in case of stack overflow error
                break;
            }
        }
        std::string stacktrace = ss.str();
        if(StringUtil::endWith(stacktrace, "\n")) {
            stacktrace = stacktrace.substr(0, stacktrace.size() - 1);
        }

        SymCleanup(process);

        urchin::Logger::instance()->logError(stacktrace);
        if(instance()->signalReceptor) {
            instance()->signalReceptor->onSignalReceived((int)exceptionInfo->ExceptionRecord->ExceptionCode);
        }

        return EXCEPTION_EXECUTE_HANDLER;
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

                long methodShift = (char*)info.dli_saddr - (char*)info.dli_fbase;
                long methodInstructionShift = (char*)traces[i] - (char*)info.dli_saddr;
                long instructionShift = methodShift + methodInstructionShift - 1; //see https://stackoverflow.com/a/63841497
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
