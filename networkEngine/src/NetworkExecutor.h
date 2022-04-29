#pragma once

#include <request/RequestInitializer.h>
#include <request/HttpRequest.h>

namespace urchin {

    class NetworkExecutor {
        public:
            NetworkExecutor();
            ~NetworkExecutor();

            void syncExecute(const HttpRequest&);
            void asyncExecute(HttpRequest);

            void interruptThread();
            void checkNoExceptionRaised() const;

        private:
            void start();
            bool continueExecution() const;

            std::unique_ptr<std::jthread> networkThread;
            std::atomic_bool networkThreadStopper;
            static std::exception_ptr networkThreadExceptionPtr;

            std::thread::id mainThreadId;
    };

}
