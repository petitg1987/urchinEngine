#ifndef URCHINENGINE_AIMANAGER_H
#define URCHINENGINE_AIMANAGER_H

#include <vector>
#include <atomic>
#include <thread>
#include <mutex>
#include "UrchinCommon.h"

#include "input/AIWorld.h"
#include "path/PathRequest.h"
#include "path/navmesh/NavMeshGenerator.h"
#include "path/navmesh/model/NavMesh.h"
#include "path/navmesh/model/NavMeshConfig.h"

namespace urchin
{

    class AIManager
    {
        public:
            AIManager();
            ~AIManager();

            NavMeshGenerator *getNavMeshGenerator() const;

            void setAIWorld(const std::shared_ptr<AIWorld> &);
            void addPathRequest(const std::shared_ptr<PathRequest> &);

            void start(float, unsigned int maxStep = 1);
            void pause();
            void play();
            bool isPaused() const;
            void interrupt();

        private:
            void startAIUpdate();
            bool continueExecution();
            void processAIUpdate();

            std::thread *aiSimulationThread;
            std::atomic_bool aiSimulationStopper;

            mutable std::mutex mutex;
            float timeStep;
            unsigned int maxSubStep;
            bool paused;

            NavMeshGenerator *navMeshGenerator;
            std::shared_ptr<AIWorld> aiWorld;
            std::vector<std::shared_ptr<PathRequest>> pathRequests;
    };

}

#endif
