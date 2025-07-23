#include <UrchinCommon.h>

#include "AIEnvironment.h"
#include "path/pathfinding/PathfindingAStar.h"

namespace urchin {

    //static
    std::exception_ptr AIEnvironment::aiThreadExceptionPtr = nullptr;

    AIEnvironment::AIEnvironment() :
            aiSimulationThread(nullptr),
            aiSimulationStopper(false),
            timeStep(0),
            paused(true),
            navMeshGenerator(NavMeshGenerator()) {
        SignalHandler::instance().initialize();
    }

    AIEnvironment::~AIEnvironment() {
        if (aiSimulationThread) {
            interruptThread();
            aiSimulationThread->join();
        }

        copiedPathRequests.clear();
        pathRequests.clear();
    }

    NavMeshGenerator& AIEnvironment::getNavMeshGenerator() {
        return navMeshGenerator;
    }

    void AIEnvironment::addEntity(const std::shared_ptr<AIEntity>& aiEntity) {
        aiWorld.addEntity(aiEntity);
    }

    void AIEnvironment::removeEntity(const std::shared_ptr<AIEntity>& aiEntity) {
        aiWorld.removeEntity(aiEntity);
    }

    void AIEnvironment::addPathRequest(const std::shared_ptr<PathRequest>& pathRequest) {
        std::scoped_lock lock(mutex);
        pathRequests.push_back(pathRequest);
    }

    void AIEnvironment::removePathRequest(const PathRequest& pathRequest) {
        std::scoped_lock lock(mutex);

        auto itFind = std::ranges::find_if(pathRequests, [&pathRequest](const auto& o){return o.get() == &pathRequest;});
        if (itFind != pathRequests.end()) {
            VectorUtil::erase(pathRequests, itFind);
        }
    }

    /**
     * Set up the AI simulation in new thread
     * @param timeStep Frequency updates expressed in second
     */
    void AIEnvironment::setUp(float timeStep) {
        if (aiSimulationThread) {
            throw std::runtime_error("AI thread is already started");
        }

        this->timeStep = timeStep;
        aiSimulationThread = std::make_unique<std::jthread>(&AIEnvironment::startAIUpdate, this);
    }

    void AIEnvironment::pause() {
        std::scoped_lock lock(mutex);
        paused = true;
    }

    void AIEnvironment::unpause() {
        std::scoped_lock lock(mutex);
        paused = false;
    }

    bool AIEnvironment::isPaused() const {
        std::scoped_lock lock(mutex);
        return paused;
    }

    void AIEnvironment::interruptThread() {
        aiSimulationStopper.store(true, std::memory_order_release);
    }

    /**
     * Check if thread has been stopped by an exception and rethrow exception on main thread
     */
    void AIEnvironment::checkNoExceptionRaised() const {
        if (aiThreadExceptionPtr) {
            std::rethrow_exception(aiThreadExceptionPtr);
        }
    }

    void AIEnvironment::startAIUpdate() {
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
                    if (SleepUtil::stepSleep((int)(remainingTime * 1000.0f), this)) {
                        break;
                    }
                    frameStartTime = std::chrono::steady_clock::now();
                } else {
                    frameStartTime = frameEndTime;
                }
            }

            Profiler::ai().log(); //log for AI thread
        } catch (const std::exception&) {
            Logger::instance().logError("Error cause AI thread crash: exception reported to main thread");
            aiThreadExceptionPtr = std::current_exception();
        }
    }

    /**
     * @return True if thread execution is not interrupted
    */
    bool AIEnvironment::continueExecution() const {
        return !aiSimulationStopper.load(std::memory_order_acquire);
    }

    void AIEnvironment::processAIUpdate() {
        ScopeProfiler sp(Profiler::ai(), "procAIUpdate");

        //copy for local thread
        bool paused;
        copiedPathRequests.clear();
        {
            std::scoped_lock lock(mutex);

            paused = this->paused;
            copiedPathRequests = this->pathRequests;
        }

        //AI execution
        if (!paused) {
            std::shared_ptr<NavMesh> navMesh = navMeshGenerator.generate(aiWorld);

            PathfindingAStar pathfindingAStar(navMesh);
            for (const auto& pathRequest : copiedPathRequests) {
                pathRequest->setPath(pathfindingAStar.findPath(pathRequest->getStartPoint(), pathRequest->getEndPoint()));
            }
        }
    }

}
