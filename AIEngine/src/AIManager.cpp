#include <algorithm>
#include <UrchinCommon.h>

#include <AIManager.h>
#include <path/pathfinding/PathfindingAStar.h>

namespace urchin {

    //static
    std::exception_ptr AIManager::aiThreadExceptionPtr = nullptr;

    AIManager::AIManager() :
            aiSimulationThread(nullptr),
            aiSimulationStopper(false),
            timeStep(0),
            paused(true),
            navMeshGenerator(std::make_unique<NavMeshGenerator>()) {
        NumericalCheck::perform();
        SignalHandler::instance().initialize();
    }

    AIManager::~AIManager() {
        if (aiSimulationThread) {
            interrupt();
            aiSimulationThread->join();
        }

        copiedPathRequests.clear();
        pathRequests.clear();
    }

    NavMeshGenerator& AIManager::getNavMeshGenerator() const {
        return *navMeshGenerator;
    }

    void AIManager::addEntity(const std::shared_ptr<AIEntity>& aiEntity) {
        Logger::instance().logInfo("Add new AI entity: " + aiEntity->getName());
        aiWorld.addEntity(aiEntity);
    }

    void AIManager::removeEntity(const std::shared_ptr<AIEntity>& aiEntity) {
        aiWorld.removeEntity(aiEntity);
    }

    void AIManager::addPathRequest(const std::shared_ptr<PathRequest>& pathRequest) {
        std::lock_guard<std::mutex> lock(mutex);
        pathRequests.push_back(pathRequest);
    }

    void AIManager::removePathRequest(const PathRequest& pathRequest) {
        std::lock_guard<std::mutex> lock(mutex);

        auto itFind = std::find_if(pathRequests.begin(), pathRequests.end(), [&pathRequest](const auto& o){return o.get() == &pathRequest;});
        if (itFind != pathRequests.end()) {
            VectorUtil::erase(pathRequests, itFind);
        }
    }

    /**
     * Set up the AI simulation in new thread
     * @param timeStep Frequency updates expressed in second
     */
    void AIManager::setUp(float timeStep) {
        if (aiSimulationThread) {
            throw std::runtime_error("AI thread is already started");
        }

        this->timeStep = timeStep;
        aiSimulationThread = std::make_unique<std::thread>(&AIManager::startAIUpdate, this);
    }

    void AIManager::pause() {
        std::lock_guard<std::mutex> lock(mutex);
        paused = true;
    }

    void AIManager::unpause() {
        std::lock_guard<std::mutex> lock(mutex);
        paused = false;
    }

    bool AIManager::isPaused() const {
        std::lock_guard<std::mutex> lock(mutex);
        return paused;
    }

    /**
     * Interrupt the thread
     */
    void AIManager::interrupt() {
        aiSimulationStopper.store(true, std::memory_order_release);
    }

    /**
     * Check if thread has been stopped by an exception and rethrow exception on main thread
     */
    void AIManager::checkNoExceptionRaised() {
        if (aiThreadExceptionPtr) {
            std::rethrow_exception(aiThreadExceptionPtr);
        }
    }

    void AIManager::startAIUpdate() {
        try {
            auto frameStartTime = std::chrono::steady_clock::now();

            while (continueExecution()) {
                processAIUpdate();

                auto frameEndTime = std::chrono::steady_clock::now();
                auto deltaTimeInUs = std::chrono::duration_cast<std::chrono::microseconds>(frameEndTime - frameStartTime).count();
                if (deltaTimeInUs < 250) { //small delta time on Windows is imprecise: wait two milliseconds more to get a more precise value
                    std::this_thread::sleep_for(std::chrono::milliseconds(2));
                    frameEndTime = std::chrono::steady_clock::now();
                    deltaTimeInUs = std::chrono::duration_cast<std::chrono::microseconds>(frameEndTime - frameStartTime).count();
                }

                float remainingTime = timeStep - (float)((double)deltaTimeInUs / 1000000.0f);
                if (remainingTime >= 0.0f) {
                    std::this_thread::sleep_for(std::chrono::milliseconds((int)(remainingTime * 1000.0f)));
                    frameStartTime = std::chrono::steady_clock::now();
                } else {
                    frameStartTime = frameEndTime;
                }
            }

            Profiler::ai().log(); //log for AI thread
        } catch (std::exception& e) {
            Logger::instance().logError("Error cause AI thread crash: exception reported to main thread");
            aiThreadExceptionPtr = std::current_exception();
        }
    }

    /**
     * @return True if thread execution is not interrupted
    */
    bool AIManager::continueExecution() {
        return !aiSimulationStopper.load(std::memory_order_acquire);
    }

    void AIManager::processAIUpdate() {
        ScopeProfiler sp(Profiler::ai(), "procAIUpdate");

        //copy for local thread
        bool paused;
        copiedPathRequests.clear();
        {
            std::lock_guard<std::mutex> lock(mutex);

            paused = this->paused;
            copiedPathRequests = this->pathRequests;
        }

        //AI execution
        if (!paused) {
            std::shared_ptr<NavMesh> navMesh = navMeshGenerator->generate(aiWorld);

            PathfindingAStar pathfindingAStar(navMesh);
            for (auto& pathRequest : copiedPathRequests) {
                pathRequest->setPath(pathfindingAStar.findPath(pathRequest->getStartPoint(), pathRequest->getEndPoint()));
            }
        }
    }

}
