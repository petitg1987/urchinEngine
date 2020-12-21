#ifndef URCHINENGINETEST_BACKTRACEHANDLER_H
#define URCHINENGINETEST_BACKTRACEHANDLER_H

#include <string>
#include <csignal>
#include <functional>
#ifdef _WIN32
    #include <windows.h>
    #include <imagehlp.h>
#else
    #include <err.h>
    #include <execinfo.h>
    #include <cxxabi.h>
#endif

namespace urchin {

    struct BacktraceReceiver {
        virtual void onSignalCaught(int) = 0;
    };

    class BacktraceHandler { //TODO rename in SignalHandler + singleton (not possible with ...) ?
        public:
            enum ErrorSimulationType {
                SEGMENTATION_FAULT,
                DIVIDE_BY_ZERO,
                EXCEPTION,
                ASSERT,
                ILLEGAL_INSTRUCTION
            };

            explicit BacktraceHandler(std::string);

            void setup(BacktraceReceiver *);

            void simulateError(ErrorSimulationType);

        private:
            void setupSignalHandler();
            #ifdef _WIN32
                static int addressToLine(void const*);
                static void windowsPrintStacktrace(CONTEXT*);
                static LONG WINAPI windowsExceptionHandler(EXCEPTION_POINTERS*);
            #else
                static void signalHandler(int, siginfo_t*, void*);
                static std::string exec(const std::string&);
            #endif

            static std::string programName;
            static BacktraceReceiver* backtraceReceiver; //TODO use unique_ptr
    };

}

#endif
