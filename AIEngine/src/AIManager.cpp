#include <utility>
#include "UrchinCommon.h"

#include "AIManager.h"
#include "path/pathfinding/PathfindingAStar.h"

namespace urchin
{

    AIManager::AIManager() :
            aiSimulationThread(nullptr),
            aiSimulationStopper(false),
            timeStep(0),
            paused(false),
            navMeshGenerator(new NavMeshGenerator())
    {
        NumericalCheck::instance()->perform();
    }

    AIManager::~AIManager()
    {
        if(aiSimulationThread!=nullptr)
        {
            interrupt();
            aiSimulationThread->join();

            delete aiSimulationThread;
        }

        delete navMeshGenerator;
    }

    NavMeshGenerator *AIManager::getNavMeshGenerator() const
    {
        return navMeshGenerator;
    }

    void AIManager::setAIWorld(const std::shared_ptr<AIWorld> &aiWorld)
    {
        std::lock_guard<std::mutex> lock(mutex);

        this->aiWorld = aiWorld;
    }

    void AIManager::addPathRequest(const std::shared_ptr<PathRequest> &pathRequest)
    {
        std::lock_guard<std::mutex> lock(mutex);

        pathRequests.push_back(pathRequest);
    }

    /**
     * Launch the AI simulation in new thread
     * @param timeStep Frequency updates expressed in second
     */
    void AIManager::start(float timeStep)
    {
        if(aiSimulationThread!=nullptr)
        {
            throw std::runtime_error("AI thread is already started");
        }

        this->timeStep = timeStep;

        aiSimulationThread = new std::thread(&AIManager::startAIUpdate, this);
    }

    void AIManager::pause()
    {
        std::lock_guard<std::mutex> lock(mutex);

        paused = true;
    }

    void AIManager::play()
    {
        std::lock_guard<std::mutex> lock(mutex);

        paused = false;
    }

    bool AIManager::isPaused() const
    {
        std::lock_guard<std::mutex> lock(mutex);

        return paused;
    }

    /**
     * Interrupt the thread
     */
    void AIManager::interrupt()
    {
        aiSimulationStopper.store(true, std::memory_order_relaxed);
    }

    void AIManager::startAIUpdate()
    {
        try
        {
            auto frameStartTime = std::chrono::high_resolution_clock::now();

            while (continueExecution())
            {
                processAIUpdate();

                auto frameEndTime = std::chrono::high_resolution_clock::now();
                auto diffTimeMicroSeconds = std::chrono::duration_cast<std::chrono::microseconds>(frameEndTime - frameStartTime).count();
                float remainingTime = timeStep - (diffTimeMicroSeconds / 1000000.0);

                if (remainingTime >= 0.0f)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(remainingTime * 1000.0f)));

                    frameStartTime = std::chrono::high_resolution_clock::now();
                } else
                {
                    #ifdef _DEBUG
                        if(std::abs(remainingTime) > (timeStep * 0.5f))
                        {
                            Logger::logger().logWarning("Performance issues: AI engine takes " + std::to_string(remainingTime) + " seconds too long to process");
                        }
                    #endif
                    frameStartTime = frameEndTime;
                }
            }
        }catch(std::exception &e)
        {
            Logger::logger().logError("Error cause AI thread crash: " + std::string(e.what()));
            throw e;
        }
    }

    /**
     * @return True if thread execution is not interrupted
    */
    bool AIManager::continueExecution()
    {
        return !aiSimulationStopper.load(std::memory_order_relaxed);
    }

    void AIManager::processAIUpdate()
    {
        //copy for local thread
        bool paused;
        std::shared_ptr<AIWorld> aiWorld;
        std::vector<std::shared_ptr<PathRequest>> pathRequests;
        {
            std::lock_guard<std::mutex> lock(mutex);

            paused = this->paused;
            if(this->aiWorld)
            {
                aiWorld = std::make_shared<AIWorld>(*this->aiWorld);
            }
            pathRequests = this->pathRequests;
        }

        //AI execution
        if (!paused && aiWorld)
        {
            std::shared_ptr<NavMesh> navMesh = navMeshGenerator->generate(aiWorld);

            PathfindingAStar pathfindingAStar(navMesh);
            for (auto &pathRequest : pathRequests)
            {
                pathRequest->setPath(pathfindingAStar.findPath(pathRequest->getStartPoint(), pathRequest->getEndPoint()));
            }
        }
    }

}
