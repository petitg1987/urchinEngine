#pragma once

#include <vector>
#include <atomic>
#include <memory>
#include <thread>
#include <mutex>
#include <UrchinCommon.h>

#include "body/model/AbstractBody.h"
#include "body/BodyContainer.h"
#include "collision/CollisionWorld.h"
#include "raytest/RayTester.h"
#include "visualizer/CollisionVisualizer.h"

namespace urchin {

    class PhysicsWorld {
        public:
            PhysicsWorld();
            ~PhysicsWorld();

            BodyContainer& getBodyContainer();
            CollisionWorld& getCollisionWorld();

            void addBody(std::shared_ptr<AbstractBody>);
            void removeBody(const AbstractBody&);

            void triggerRayTest(std::shared_ptr<RayTester>, const Ray<float>&);

            void setGravity(const Vector3<float>&);
            Vector3<float> getGravity() const;

            void setUp(float);
            void pause();
            void unpause();
            bool isPaused() const;
            void interruptThread(bool);
            void checkNoExceptionRaised();
            const PerfMetrics& getPerfMetrics() const;

            void createCollisionVisualizer();
            const CollisionVisualizer& getCollisionVisualizer() const;

        private:
            void startPhysicsUpdate();
            bool continueExecution() const;
            void processPhysicsUpdate(float);

            void executeRayTesters(const std::vector<std::shared_ptr<RayTester>>&);

            std::unique_ptr<std::jthread> physicsSimulationThread;
            std::atomic_bool physicsSimulationStopper;
            static std::exception_ptr physicsThreadExceptionPtr;

            mutable std::mutex mutex;
            Vector3<float> gravity;
            float timeStep;
            bool paused;
            PerfMetrics perfMetrics;

            BodyContainer bodyContainer;
            CollisionWorld collisionWorld;

            std::vector<std::shared_ptr<RayTester>> rayTesters;
            std::vector<std::shared_ptr<RayTester>> threadLocalRayTesters;

            std::unique_ptr<CollisionVisualizer> collisionVisualizer;
    };

}
