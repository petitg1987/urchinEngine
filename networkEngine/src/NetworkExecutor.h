#pragma once

#include <request/RequestInitializer.h>
#include <request/HttpRequest.h>
#include <request/RequestExecutor.h>

namespace urchin {

    class NetworkExecutor {
        public:
            friend class SleepUtil;

            NetworkExecutor();
            ~NetworkExecutor();

            void asyncExecute(HttpRequest);
            RequestResult syncExecute(const HttpRequest&, unsigned int = 0);

            void interruptThread();
            void checkNoExceptionRaised() const;

        private:
            void start();
            bool continueExecution() const;

            static RequestInitializer httpRequestInitializer;
            std::thread::id mainThreadId;
            int executionPauseTime;

            std::unique_ptr<std::jthread> networkThread;
            std::atomic_bool networkThreadStopper;
            static std::exception_ptr networkThreadExceptionPtr;

            CircularFifo<HttpRequest, 16> asyncRequestsQueue;
            RequestExecutor asyncRequestExecutor;
            RequestExecutor syncRequestExecutor;
    };

}
