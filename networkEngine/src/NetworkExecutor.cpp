#include <NetworkExecutor.h>

namespace urchin {

    //static
    std::exception_ptr NetworkExecutor::networkThreadExceptionPtr = nullptr;

    NetworkExecutor::NetworkExecutor() {
        SignalHandler::instance().initialize();

        static RequestInitializer httpRequestInitializer;
        networkThread = std::make_unique<std::jthread>(&NetworkExecutor::start, this);

        mainThreadId = std::this_thread::get_id();
    }

    NetworkExecutor::~NetworkExecutor() {
        if (networkThread) {
            interruptThread();
            networkThread->join();
        }
    }

    RequestResult NetworkExecutor::syncExecute(const HttpRequest& httpRequest) {
        #ifdef URCHIN_DEBUG
            assert(std::this_thread::get_id() == mainThreadId); //not sure if it is mandatory but added for security
        #endif
        return syncRequestExecutor.executeRequest(httpRequest);
    }

    void NetworkExecutor::asyncExecute(HttpRequest) {
        //TODO impl
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
                //TODO execute async request
                //TODO pause or wake up ?
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
