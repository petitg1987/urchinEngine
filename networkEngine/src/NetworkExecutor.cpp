#include <NetworkExecutor.h>

namespace urchin {

    //static
    std::exception_ptr NetworkExecutor::networkThreadExceptionPtr = nullptr;

    NetworkExecutor::NetworkExecutor() {
        SignalHandler::instance().initialize();

        static RequestInitializer httpRequestInitializer;
        networkThread = std::make_unique<std::jthread>(&NetworkExecutor::start, this);
    }

    NetworkExecutor::~NetworkExecutor() {
        if (networkThread) {
            interrupt();
            networkThread->join();
        }
    }

    void NetworkExecutor::syncExecute(const HttpRequest&) {
        //TODO ...
    }

    void NetworkExecutor::asyncExecute(HttpRequest) {
        //TODO ...
    }

    /**
     * Interrupt the thread
     */
    void NetworkExecutor::interrupt() {
        networkThreadStopper.store(true, std::memory_order_release);
    }

    /**
     * Check if thread has been stopped by an exception and rethrow exception on main thread
     */
    void NetworkExecutor::checkNoExceptionRaised() const { //TODO call it
        if (networkThreadExceptionPtr) {
            std::rethrow_exception(networkThreadExceptionPtr);
        }
    }

    void NetworkExecutor::start() {
        try {
            Logger::instance().logInfo("Network thread started");

            while (continueExecution()) {
                //TODO execute async request
                //TODO pause
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
