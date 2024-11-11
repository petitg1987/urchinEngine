#pragma once

#include <vector>
#include <atomic>
#include <thread>
#include <mutex>

#include <input/AIWorld.h>
#include <input/AIEntity.h>
#include <path/PathRequest.h>
#include <path/navmesh/NavMeshGenerator.h>

namespace urchin {

    class AIEnvironment {
        public:
            friend class SleepUtil;

            AIEnvironment();
            ~AIEnvironment();

            NavMeshGenerator& getNavMeshGenerator();

            void addEntity(const std::shared_ptr<AIEntity>&);
            void removeEntity(const std::shared_ptr<AIEntity>&);

            void addPathRequest(const std::shared_ptr<PathRequest>&);
            void removePathRequest(const PathRequest&);

            void setUp(float);
            void pause();
            void unpause();
            bool isPaused() const;
            void interruptThread();
            void checkNoExceptionRaised() const;

        private:
            void startAIUpdate();
            bool continueExecution() const;
            void processAIUpdate();

            std::unique_ptr<std::jthread> aiSimulationThread;
            std::atomic_bool aiSimulationStopper;
            static std::exception_ptr aiThreadExceptionPtr;

            mutable std::mutex mutex;
            float timeStep;
            bool paused;

            NavMeshGenerator navMeshGenerator;
            AIWorld aiWorld;
            std::vector<std::shared_ptr<PathRequest>> pathRequests;
            std::vector<std::shared_ptr<PathRequest>> copiedPathRequests;
    };

}
