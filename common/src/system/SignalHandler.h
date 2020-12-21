#ifndef URCHINENGINETEST_BACKTRACEHANDLER_H
#define URCHINENGINETEST_BACKTRACEHANDLER_H

#include <string>
#include <csignal>
#include <memory>
#include <functional>
#ifdef _WIN32
    #include <winsock2.h> //only to avoid inclusion order error in others parts of the engine
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
                static LONG WINAPI signalHandler(EXCEPTION_POINTERS*);
            #else
                static void signalHandler(int, siginfo_t*, void*);
                std::string executeCommand(const std::string&);
            #endif

            std::unique_ptr<SignalReceptor> signalReceptor;
    };

}

#endif
