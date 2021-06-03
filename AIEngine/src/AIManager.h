#ifndef URCHINENGINE_AIMANAGER_H
#define URCHINENGINE_AIMANAGER_H

#include <vector>
#include <atomic>
#include <thread>
#include <mutex>
#include <UrchinCommon.h>

#include <input/AIWorld.h>
#include <input/AIEntity.h>
#include <path/PathRequest.h>
#include <path/navmesh/NavMeshGenerator.h>
#include <path/navmesh/model/output/NavMesh.h>

namespace urchin {

    class AIManager {
        public:
            AIManager();
            ~AIManager();

            NavMeshGenerator* getNavMeshGenerator() const;

            void addEntity(const std::shared_ptr<AIEntity>&);
            void removeEntity(const std::shared_ptr<AIEntity>&);

            void addPathRequest(const std::shared_ptr<PathRequest>&);
            void removePathRequest(const std::shared_ptr<PathRequest>&);

            void setUp(float);
            void pause();
            void unpause();
            bool isPaused() const;
            void interrupt();
            void controlExecution();

        private:
            void startAIUpdate();
            bool continueExecution();
            void processAIUpdate();

            std::thread *aiSimulationThread;
            std::atomic_bool aiSimulationStopper;
            static std::exception_ptr aiThreadExceptionPtr;

            mutable std::mutex mutex;
            float timeStep;
            bool paused;

            NavMeshGenerator* navMeshGenerator;
            AIWorld aiWorld;
            std::vector<std::shared_ptr<PathRequest>> pathRequests;
            std::vector<std::shared_ptr<PathRequest>> copiedPathRequests;
    };

}

#endif
