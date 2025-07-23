#include "NetworkExecutor.h"

namespace urchin {

    //static
    std::exception_ptr NetworkExecutor::networkThreadExceptionPtr = nullptr;

    NetworkExecutor::NetworkExecutor() :
            mainThreadId(std::this_thread::get_id()),
            executionPauseTime((int)ConfigService::instance().getUnsignedIntValue("network.executionPauseTime")) {
        SignalHandler::instance().initialize();

        networkThread = std::make_unique<std::jthread>(&NetworkExecutor::start, this);
    }

    NetworkExecutor::~NetworkExecutor() {
        if (networkThread) {
            interruptThread();
            networkThread->join();
        }
    }

    void NetworkExecutor::asyncExecute(HttpRequest httpRequest) {
        bool pushSuccess = asyncRequestsQueue.push(std::move(httpRequest));
        if (!pushSuccess) {
            Logger::instance().logError("Requests queue is full: cannot execute more requests");
        }
    }

    RequestResult NetworkExecutor::syncExecute(const HttpRequest& httpRequest, unsigned int timoutSec) const {
        #ifdef URCHIN_DEBUG
            assert(std::this_thread::get_id() == mainThreadId); //not sure if it is mandatory but added for security
        #endif
        return syncRequestExecutor.executeRequest(httpRequest, timoutSec);
    }

    void NetworkExecutor::interruptThread() {
        networkThreadStopper.store(true, std::memory_order_release);
    }

    /**
     * Check if thread has been stopped by an exception and rethrow exception on main thread
     */
    void NetworkExecutor::checkNoExceptionRaised() const {
        if (networkThreadExceptionPtr) {
            std::rethrow_exception(networkThreadExceptionPtr);
        }
    }

    void NetworkExecutor::start() {
        try {
            Logger::instance().logInfo("Network thread started");

            while (continueExecution()) {
                HttpRequest httpRequest;
                while (asyncRequestsQueue.pop(httpRequest)) {
                    asyncRequestExecutor.executeRequest(httpRequest);
                }

                if (SleepUtil::stepSleep(executionPauseTime, this)) {
                    break;
                }
            }

            Profiler::network().log(); //log for network thread
        } catch (const std::exception&) {
            Logger::instance().logError("Error cause network thread crash: exception reported to main thread");
            networkThreadExceptionPtr = std::current_exception();
        }
    }

    /**
     * @return True if thread execution is not interrupted
     */
    bool NetworkExecutor::continueExecution() const {
        return !networkThreadStopper.load(std::memory_order_acquire);
    }

}
