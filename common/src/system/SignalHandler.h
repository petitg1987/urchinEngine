#ifndef URCHINENGINETEST_BACKTRACEHANDLER_H
#define URCHINENGINETEST_BACKTRACEHANDLER_H

#include <string>
#include <csignal>
#include <memory>

#include "pattern/singleton/Singleton.h"

#ifdef _WIN32
    struct _EXCEPTION_POINTERS;
#endif

namespace urchin {

    class SignalReceptor {
        public:
            virtual void onSignalReceived(unsigned long) = 0;
    };

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

            void initialize();
            void registerSignalReceptor(const std::shared_ptr<SignalReceptor>&);

            void simulateError(ErrorSimulationType);

        private:
            SignalHandler();
            ~SignalHandler() override = default;

            void setupSignalHandler();
            #ifdef _WIN32
                static long __stdcall signalHandler(_EXCEPTION_POINTERS*);
            #else
                static void signalHandler(int, siginfo_t*, void*);
                std::string executeCommand(const std::string&);
            #endif

            bool isInitialized;
            std::shared_ptr<SignalReceptor> signalReceptor;
    };

}

#endif
