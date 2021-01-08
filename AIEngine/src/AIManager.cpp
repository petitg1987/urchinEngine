#include <algorithm>
#include "UrchinCommon.h"

#include "AIManager.h"
#include "path/pathfinding/PathfindingAStar.h"

namespace urchin {

    //static
    std::exception_ptr AIManager::aiThreadExceptionPtr = nullptr;

    AIManager::AIManager() :
            aiSimulationThread(nullptr),
            aiSimulationStopper(false),
            timeStep(0),
            paused(true),
            navMeshGenerator(new NavMeshGenerator()) {
        NumericalCheck::perform();
        SignalHandler::instance()->initialize();
    }

    AIManager::~AIManager() {
        if (aiSimulationThread) {
            interrupt();
            aiSimulationThread->join();

            delete aiSimulationThread;
        }

        copiedPathRequests.clear();
        pathRequests.clear();

        delete navMeshGenerator;

        Profiler::ai()->log();
    }

    NavMeshGenerator* AIManager::getNavMeshGenerator() const {
        return navMeshGenerator;
    }

    void AIManager::addEntity(const std::shared_ptr<AIEntity>& aiEntity) {
        aiWorld.addEntity(aiEntity);
    }

    void AIManager::removeEntity(const std::shared_ptr<AIEntity>& aiEntity) {
        aiWorld.removeEntity(aiEntity);
    }

    void AIManager::addPathRequest(const std::shared_ptr<PathRequest>& pathRequest) {
        std::lock_guard<std::mutex> lock(mutex);

        pathRequests.push_back(pathRequest);
    }

    void AIManager::removePathRequest(const std::shared_ptr<PathRequest>& pathRequest) {
        std::lock_guard<std::mutex> lock(mutex);

        if (pathRequest) {
            auto itFind = std::find(pathRequests.begin(), pathRequests.end(), pathRequest);
            if (itFind != pathRequests.end()) {
                VectorUtil::erase(pathRequests, itFind);
            }
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

        aiSimulationThread = new std::thread(&AIManager::startAIUpdate, this);
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
        aiSimulationStopper.store(true, std::memory_order_relaxed);
    }

    /**
     * Check if thread has been stopped by an exception and rethrow exception on main thread
     */
    void AIManager::controlExecution() {
        if (aiThreadExceptionPtr) {
            std::rethrow_exception(aiThreadExceptionPtr);
        }
    }

    void AIManager::startAIUpdate() {
        try {
            auto frameStartTime = std::chrono::high_resolution_clock::now();

            while (continueExecution()) {
                processAIUpdate();

                auto frameEndTime = std::chrono::high_resolution_clock::now();
                auto diffTimeMicroSeconds = std::chrono::duration_cast<std::chrono::microseconds>(frameEndTime - frameStartTime).count();
                float remainingTime = timeStep - (float)((double)diffTimeMicroSeconds / 1000000.0f);

                if (remainingTime >= 0.0f) {
                    std::this_thread::sleep_for(std::chrono::milliseconds((int)(remainingTime * 1000.0f)));

                    frameStartTime = std::chrono::high_resolution_clock::now();
                } else {
                    frameStartTime = frameEndTime;
                }
            }
        } catch (std::exception& e) {
            Logger::instance()->logError("Error cause AI thread crash: exception reported to main thread");
            aiThreadExceptionPtr = std::current_exception();
        }
    }

    /**
     * @return True if thread execution is not interrupted
    */
    bool AIManager::continueExecution() {
        return !aiSimulationStopper.load(std::memory_order_relaxed);
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
