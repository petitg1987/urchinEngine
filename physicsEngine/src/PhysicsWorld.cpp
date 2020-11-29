#include <chrono>
#include <algorithm>

#include "PhysicsWorld.h"
#include "processable/raytest/RayTester.h"

#define DEFAULT_GRAVITY Vector3<float>(0.0f, -9.81f, 0.0f)

namespace urchin {

    //static
    std::exception_ptr PhysicsWorld::physicsThreadExceptionPtr = nullptr;

    PhysicsWorld::PhysicsWorld() :
            physicsSimulationThread(nullptr),
            physicsSimulationStopper(false),
            gravity(DEFAULT_GRAVITY),
            timeStep(0.0f),
            paused(true),
            bodyManager(new BodyManager()),
            collisionWorld(new CollisionWorld(bodyManager)),
            collisionVisualizer(nullptr) {
        NumericalCheck::instance()->perform();
    }

    PhysicsWorld::~PhysicsWorld() {
        if (physicsSimulationThread) {
            interrupt();
            physicsSimulationThread->join();

            delete physicsSimulationThread;
        }

        copiedProcessables.clear();
        processables.clear();
        oneShotProcessables.clear();

        delete collisionVisualizer;

        delete collisionWorld;
        delete bodyManager;

        Profiler::getInstance("physics")->log();
    }

    BodyManager* PhysicsWorld::getBodyManager() const {
        return bodyManager;
    }

    CollisionWorld* PhysicsWorld::getCollisionWorld() const {
        return collisionWorld;
    }

    void PhysicsWorld::addBody(AbstractBody* body) {
        if (body) {
            bodyManager->addBody(body);
        }
    }

    void PhysicsWorld::removeBody(AbstractBody* body) {
        if (body) {
            bodyManager->removeBody(body);
        }
    }

    void PhysicsWorld::addProcessable(const std::shared_ptr<Processable>& processable) {
        std::lock_guard<std::mutex> lock(mutex);

        processable->initialize(this);

        processables.push_back(processable);
    }

    void PhysicsWorld::removeProcessable(const std::shared_ptr<Processable>& processable) {
        std::lock_guard<std::mutex> lock(mutex);

        auto itFind = std::find(processables.begin(), processables.end(), processable);
        if (itFind!=processables.end()) {
            processables.erase(itFind);
        }
    }

    std::shared_ptr<const RayTestResult> PhysicsWorld::rayTest(const Ray<float>& ray) {
        std::lock_guard<std::mutex> lock(mutex);

        std::shared_ptr<RayTester> rayTester = std::make_shared<RayTester>(ray);
        rayTester->initialize(this);

        oneShotProcessables.push_back(rayTester);

        return rayTester->getRayTestResult();
    }

    /**
     * @param gravity Gravity expressed in units/s^2
     */
    void PhysicsWorld::setGravity(const Vector3<float>& gravity) {
        std::lock_guard<std::mutex> lock(mutex);

        this->gravity = gravity;
    }

    /**
     * @return gravity expressed in units/s^2
     */
    Vector3<float> PhysicsWorld::getGravity() const {
        std::lock_guard<std::mutex> lock(mutex);

        return gravity;
    }

    /**
     * Set up the physics simulation in new thread
     * @param timeStep Frequency updates expressed in second
     */
    void PhysicsWorld::setUp(float timeStep) {
        if (physicsSimulationThread) {
            throw std::runtime_error("Physics thread is already started");
        }

        this->timeStep = timeStep;

        physicsSimulationThread = new std::thread(&PhysicsWorld::startPhysicsUpdate, this);
    }

    void PhysicsWorld::pause() {
        std::lock_guard<std::mutex> lock(mutex);

        paused = true;
    }

    void PhysicsWorld::unpause() {
        std::lock_guard<std::mutex> lock(mutex);

        paused = false;
    }

    bool PhysicsWorld::isPaused() const {
        std::lock_guard<std::mutex> lock(mutex);

        return paused;
    }

    /**
     * Interrupt the thread
     */
    void PhysicsWorld::interrupt() {
        physicsSimulationStopper.store(true, std::memory_order_relaxed);
    }

    /**
     * Check if thread has been stopped by an exception and rethrow exception on main thread
     */
    void PhysicsWorld::controlExecution() {
        if (physicsThreadExceptionPtr) {
            std::rethrow_exception(physicsThreadExceptionPtr);
        }
    }

    void PhysicsWorld::startPhysicsUpdate() {
        try {
            float remainingTime = 0.0f;
            float maxAdditionalTimeStep = timeStep * 0.5f;
            auto frameStartTime = std::chrono::high_resolution_clock::now();

            while (continueExecution()) {
                float additionalTimeStep = std::abs(remainingTime);
                if (additionalTimeStep > maxAdditionalTimeStep) {
                    //Cannot process physics update with 'additionalTimeStep'. Value is too big and can lead to physics errors.
                    //Use 'maxAdditionalTimeStep' which lead to slow-down of the physics.
                    additionalTimeStep = maxAdditionalTimeStep;
                }

                processPhysicsUpdate(timeStep + additionalTimeStep);

                auto frameEndTime = std::chrono::high_resolution_clock::now();
                auto diffTimeMicroSeconds = std::chrono::duration_cast<std::chrono::microseconds>(frameEndTime - frameStartTime).count();
                remainingTime = (timeStep + additionalTimeStep) - static_cast<float>(diffTimeMicroSeconds / 1000000.0);

                if (remainingTime >= 0.0f) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(remainingTime * 1000.0f)));

                    remainingTime = 0.0f;
                    frameStartTime = std::chrono::high_resolution_clock::now();
                } else {
                    frameStartTime = frameEndTime;
                }
            }
        } catch (std::exception& e) {
            Logger::logger().logError("Error cause physics thread crash: exception reported to main thread");
            physicsThreadExceptionPtr = std::current_exception();
        }
    }

    /**
     * @return True if thread execution is not interrupted
     */
    bool PhysicsWorld::continueExecution() {
        return !physicsSimulationStopper.load(std::memory_order_relaxed);
    }

    void PhysicsWorld::processPhysicsUpdate(float frameTimeStep) {
        ScopeProfiler profiler("physics", "procPhysicsUp");

        //copy for local thread
        bool paused;
        Vector3<float> gravity;
        copiedProcessables.clear();
        {
            std::lock_guard<std::mutex> lock(mutex);

            paused = this->paused;

            gravity = this->gravity;

            copiedProcessables = this->processables;
            copiedProcessables.insert(copiedProcessables.end(), oneShotProcessables.begin(), oneShotProcessables.end());
            oneShotProcessables.clear();
        }

        //physics execution
        if (!paused) {
            setupProcessables(copiedProcessables, frameTimeStep, gravity);

            collisionWorld->process(frameTimeStep, gravity);

            executeProcessables(copiedProcessables, frameTimeStep, gravity);
        }
    }

    /**
     * @param dt Delta of time between two simulation steps
     * @param gravity Gravity expressed in units/s^2
     */
    void PhysicsWorld::setupProcessables(const std::vector<std::shared_ptr<Processable>>& processables, float dt, const Vector3<float>& gravity) {
        ScopeProfiler profiler("physics", "stpProcessable");

        for (const auto& processable : processables) {
            processable->setup(dt, gravity);
        }
    }

    /**
     * @param dt Delta of time between two simulation steps
     * @param gravity Gravity expressed in units/s^2
     */
    void PhysicsWorld::executeProcessables(const std::vector<std::shared_ptr<Processable>>& processables, float dt, const Vector3<float>& gravity) {
        ScopeProfiler profiler("physics", "execProcessable");

        for (const auto& processable : processables) {
            processable->execute(dt, gravity);
        }
    }

    void PhysicsWorld::createCollisionVisualizer() {
        if (!collisionVisualizer) {
            collisionVisualizer = new CollisionVisualizer(collisionWorld);
        }
    }

    const CollisionVisualizer* PhysicsWorld::getCollisionVisualizer() const {
        if (!collisionVisualizer) {
            throw std::runtime_error("Impossible to get collision visualizer because not created.");
        }

        return collisionVisualizer;
    }

}
