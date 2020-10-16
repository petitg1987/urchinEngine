#ifndef URCHINENGINE_PHYSICSWORLD_H
#define URCHINENGINE_PHYSICSWORLD_H

#include <vector>
#include <atomic>
#include <memory>
#include <thread>
#include <mutex>
#include "UrchinCommon.h"

#include "body/model/AbstractBody.h"
#include "body/BodyManager.h"
#include "collision/CollisionWorld.h"
#include "processable/Processable.h"
#include "processable/raytest/RayTestResult.h"
#include "visualizer/CollisionVisualizer.h"

namespace urchin {

    class PhysicsWorld {
        public:
            PhysicsWorld();
            ~PhysicsWorld();

            BodyManager *getBodyManager() const;
            CollisionWorld *getCollisionWorld() const;

            void addBody(AbstractBody *);
            void removeBody(AbstractBody *);

            void addProcessable(const std::shared_ptr<Processable> &);
            void removeProcessable(const std::shared_ptr<Processable> &);

            std::shared_ptr<const RayTestResult> rayTest(const Ray<float> &);

            void setGravity(const Vector3<float> &);
            Vector3<float> getGravity() const;

            void setUp(float);
            void pause();
            void unpause();
            bool isPaused() const;
            void interrupt();
            void controlExecution();

            void createCollisionVisualizer();
            const CollisionVisualizer *getCollisionVisualizer() const;

        private:
            void startPhysicsUpdate();
            bool continueExecution();
            void processPhysicsUpdate(float);

            void setupProcessables(const std::vector<std::shared_ptr<Processable>> &, float, const Vector3<float> &);
            void executeProcessables(const std::vector<std::shared_ptr<Processable>> &, float, const Vector3<float> &);

            std::thread *physicsSimulationThread;
            std::atomic_bool physicsSimulationStopper;
            static std::exception_ptr physicsThreadExceptionPtr;

            mutable std::mutex mutex;
            Vector3<float> gravity;
            float timeStep;
            bool paused;

            BodyManager *bodyManager;
            CollisionWorld *collisionWorld;

            std::vector<std::shared_ptr<Processable>> processables;
            std::vector<std::shared_ptr<Processable>> oneShotProcessables;
            std::vector<std::shared_ptr<Processable>> copiedProcessables;

            CollisionVisualizer *collisionVisualizer;
    };

}

#endif
