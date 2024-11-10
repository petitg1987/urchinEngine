#pragma once

#include <csignal>
#include <memory>

#include <pattern/singleton/Singleton.h>

#ifdef _WIN32
    struct _EXCEPTION_POINTERS;
#endif

namespace urchin {

    class SignalReceptor {
        public:
            virtual ~SignalReceptor() = default;

            virtual void onSignalReceived(unsigned long) = 0;
    };

    class SignalHandler final : public Singleton<SignalHandler> {
        public:
            friend class Singleton;

            enum ErrorSimulationType {
                SEGMENTATION_FAULT,
                DIVIDE_BY_ZERO,
                EXCEPTION, //only when uncaught
                ASSERT,
                ILLEGAL_INSTRUCTION
            };

            void initialize();
            void registerSignalReceptor(const std::shared_ptr<SignalReceptor>&);

            void simulateError(ErrorSimulationType) const;

        private:
            SignalHandler();

            void setupSignalHandler() const;
            #ifdef _WIN32
                static long __stdcall signalHandler(_EXCEPTION_POINTERS*);
            #else
                static void signalHandler(int, siginfo_t*, void*);
            #endif

            bool isInitialized;
            std::shared_ptr<SignalReceptor> signalReceptor;
    };

}
