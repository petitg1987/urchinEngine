#ifndef URCHINENGINETEST_BACKTRACEHANDLER_H
#define URCHINENGINETEST_BACKTRACEHANDLER_H

#include <string>
#include <csignal>
#include <memory>
#include <functional>
#ifdef _WIN32
    #include <windows.h>
    #include <imagehlp.h>
#else
    #include <err.h>
    #include <execinfo.h>
    #include <cxxabi.h>
#endif

#include "pattern/singleton/Singleton.h"

namespace urchin {

    class SignalReceptor {
        public:
            virtual void onSignalReceived(int) = 0;
    };

    //TODO add "-g" symbol for Release
    //TODO init SignalHandler in urchin
    class SignalHandler : public Singleton<SignalHandler> {
        public:
            friend class Singleton<SignalHandler>;

            enum ErrorSimulationType {
                SEGMENTATION_FAULT,
                DIVIDE_BY_ZERO,
                EXCEPTION, //only when uncaught
                ASSERT,
                ILLEGAL_INSTRUCTION
            };

            void registerSignalReceptor(std::unique_ptr<SignalReceptor>&&);
            void simulateError(ErrorSimulationType);

        private:
            SignalHandler();
            ~SignalHandler() override = default;

            void setupSignalHandler();
            #ifdef _WIN32
                int addressToLine(void const*);
                void windowsPrintStacktrace(CONTEXT*);
                static LONG WINAPI windowsExceptionHandler(EXCEPTION_POINTERS*);
            #else
                static void signalHandler(int, siginfo_t*, void*);
                std::string executeCommand(const std::string&);
            #endif

            std::unique_ptr<SignalReceptor> signalReceptor;
    };

}

#endif
