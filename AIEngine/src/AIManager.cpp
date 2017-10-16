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
            maxSubStep(1),
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
     * @param maxSubStep Maximum number of sub steps to execute
     */
    void AIManager::start(float timeStep, unsigned int maxSubStep)
    {
        if(aiSimulationThread!=nullptr)
        {
            throw std::runtime_error("AI thread is already started");
        }

        this->timeStep = timeStep;
        this->maxSubStep = maxSubStep;

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
            double remainingTime = 0.0;
            auto frameStartTime = std::chrono::high_resolution_clock::now();

            while (continueExecution())
            {
                //Compute number of sub steps to execute.
                //Note: rest of division could be used in next steps and interpolate between steps (see http://gafferongames.com/game-physics/fix-your-timestep/)
                auto numSteps = static_cast<unsigned int>((timeStep - remainingTime) / timeStep);
                //Clamp number of sub steps to max sub step to avoid spiral of death (time of simulation increase and need to be executed increasingly).
                unsigned int numStepsClamped = (numSteps > maxSubStep) ? maxSubStep : numSteps;

                for (unsigned int step = 0; step < numStepsClamped; ++step)
                {
                    processAIUpdate();
                }

                auto frameEndTime = std::chrono::high_resolution_clock::now();
                auto diffTimeMicroSeconds = std::chrono::duration_cast<std::chrono::microseconds>(frameEndTime - frameStartTime).count();
                remainingTime = timeStep - (diffTimeMicroSeconds / 1000000.0);

                if (remainingTime >= 0.0)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(remainingTime * 1000.0)));

                    remainingTime = 0.0;
                    frameStartTime = std::chrono::high_resolution_clock::now();
                } else
                {
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
