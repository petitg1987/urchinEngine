#include <chrono>
#include <algorithm>

#include <PhysicsWorld.h>
#include <processable/raytest/RayTester.h>

namespace urchin {

    //static
    std::exception_ptr PhysicsWorld::physicsThreadExceptionPtr = nullptr;

    PhysicsWorld::PhysicsWorld() :
            physicsSimulationThread(std::unique_ptr<std::jthread>(nullptr)),
            physicsSimulationStopper(false),
            gravity(Vector3(0.0f, -9.81f, 0.0f)),
            timeStep(0.0f),
            paused(true),
            bodyContainer(BodyContainer()),
            collisionWorld(CollisionWorld(getBodyContainer())) {
        SignalHandler::instance().initialize();
    }

    PhysicsWorld::~PhysicsWorld() {
        if (physicsSimulationThread) {
            interruptThread();
            physicsSimulationThread->join();
        }

        copiedProcessables.clear();
        processables.clear();
        oneShotProcessables.clear();

        Profiler::physics().log(); //log for main (not physics) thread
    }

    BodyContainer& PhysicsWorld::getBodyContainer() {
        return bodyContainer;
    }

    CollisionWorld& PhysicsWorld::getCollisionWorld() {
        return collisionWorld;
    }

    void PhysicsWorld::addBody(std::shared_ptr<AbstractBody> body) {
        if (body) {
            bodyContainer.addBody(std::move(body));
        }
    }

    void PhysicsWorld::removeBody(const AbstractBody& body) {
        bodyContainer.removeBody(body);
    }

    void PhysicsWorld::addProcessable(const std::shared_ptr<Processable>& processable) {
        std::scoped_lock lock(mutex);
        processables.push_back(processable);
    }

    void PhysicsWorld::removeProcessable(const Processable& processable) {
        std::scoped_lock lock(mutex);

        auto itFind = std::ranges::find_if(processables, [&processable](const auto& o){return o.get() == &processable;});
        if (itFind != processables.end()) {
            processables.erase(itFind);
        }
    }

    std::shared_ptr<const RayTestResult> PhysicsWorld::rayTest(const Ray<float>& ray) {
        std::scoped_lock lock(mutex);

        auto rayTester = std::make_unique<RayTester>(getCollisionWorld(), ray);
        std::shared_ptr<const RayTestResult> rayTestResult = rayTester->getRayTestResult();
        oneShotProcessables.push_back(std::move(rayTester));

        return rayTestResult;
    }

    /**
     * @param gravity Gravity expressed in units/s^2
     */
    void PhysicsWorld::setGravity(const Vector3<float>& gravity) {
        std::scoped_lock lock(mutex);
        this->gravity = gravity;
    }

    /**
     * @return gravity expressed in units/s^2
     */
    Vector3<float> PhysicsWorld::getGravity() const {
        std::scoped_lock lock(mutex);
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
        physicsSimulationThread = std::make_unique<std::jthread>(&PhysicsWorld::startPhysicsUpdate, this);
    }

    void PhysicsWorld::pause() {
        std::scoped_lock lock(mutex);
        paused = true;
    }

    void PhysicsWorld::unpause() {
        std::scoped_lock lock(mutex);
        paused = false;
    }

    bool PhysicsWorld::isPaused() const {
        std::scoped_lock lock(mutex);
        return paused;
    }

    void PhysicsWorld::interruptThread() {
        physicsSimulationStopper.store(true, std::memory_order_release);
    }

    /**
     * Check if thread has been stopped by an exception and rethrow exception on main thread
     */
    void PhysicsWorld::checkNoExceptionRaised() const {
        if (physicsThreadExceptionPtr) {
            std::rethrow_exception(physicsThreadExceptionPtr);
        }
    }

    void PhysicsWorld::startPhysicsUpdate() {
        try {
            Logger::instance().logInfo("Physics thread started with time step of " + std::to_string(timeStep) + " sec");

            float remainingTime = 0.0f;
            float maxAdditionalTimeStep = timeStep * 0.5f;
            auto frameStartTime = std::chrono::steady_clock::now();

            while (continueExecution()) {
                float additionalTimeStep = std::abs(remainingTime);
                if (additionalTimeStep > maxAdditionalTimeStep) {
                    //Cannot process physics update with 'additionalTimeStep'. Value is too big and can lead to physics errors.
                    //Use 'maxAdditionalTimeStep' which lead to slow-down of the physics.
                    additionalTimeStep = maxAdditionalTimeStep;
                }

                processPhysicsUpdate(timeStep + additionalTimeStep);

                auto frameEndTime = std::chrono::steady_clock::now();
                auto deltaTimeInUs = std::chrono::duration_cast<std::chrono::microseconds>(frameEndTime - frameStartTime).count();
                if (deltaTimeInUs < 250) { //small delta time on Windows is imprecise: wait two milliseconds more to get a more precise value
                    std::this_thread::sleep_for(std::chrono::milliseconds(2));
                    frameEndTime = std::chrono::steady_clock::now();
                    deltaTimeInUs = std::chrono::duration_cast<std::chrono::microseconds>(frameEndTime - frameStartTime).count();
                }

                remainingTime = (timeStep + additionalTimeStep) - (float)((double)deltaTimeInUs / 1000000.0);
                if (remainingTime >= 0.0f) {
                    std::this_thread::sleep_for(std::chrono::milliseconds((int)(remainingTime * 1000.0f)));
                    remainingTime = 0.0f;
                    frameStartTime = std::chrono::steady_clock::now();
                } else {
                    frameStartTime = frameEndTime;
                }
            }

            Profiler::physics().log(); //log for physics thread
        } catch (const std::exception&) {
            Logger::instance().logError("Error cause physics thread crash: exception reported to main thread");
            physicsThreadExceptionPtr = std::current_exception();
        }
    }

    /**
     * @return True if thread execution is not interrupted
     */
    bool PhysicsWorld::continueExecution() const {
        return !physicsSimulationStopper.load(std::memory_order_acquire);
    }

    void PhysicsWorld::processPhysicsUpdate(float dt) {
        ScopeProfiler sp(Profiler::physics(), "procPhysicsUp");

        //copy for local thread
        bool paused;
        Vector3<float> gravity;
        copiedProcessables.clear();

        {
            std::scoped_lock lock(mutex);
            paused = this->paused;
            if (!paused) {
                gravity = this->gravity;
                copiedProcessables = this->processables;
                copiedProcessables.reserve(copiedProcessables.size() + oneShotProcessables.size());
                for (auto& oneShotProcessable : oneShotProcessables) {
                    copiedProcessables.push_back(std::move(oneShotProcessable));
                }
                oneShotProcessables.clear();
            }
        }

        //physics execution
        if (!paused) {
            setupProcessables(copiedProcessables, dt, gravity);

            collisionWorld.process(dt, gravity);

            executeProcessables(copiedProcessables, dt, gravity);
        }
    }

    /**
     * @param dt Delta of time between two simulation steps
     * @param gravity Gravity expressed in units/s^2
     */
    void PhysicsWorld::setupProcessables(const std::vector<std::shared_ptr<Processable>>& processables, float dt, const Vector3<float>& gravity) const {
        ScopeProfiler sp(Profiler::physics(), "stpProcessable");

        for (const auto& processable : processables) {
            processable->setup(dt, gravity);
        }
    }

    /**
     * @param dt Delta of time between two simulation steps
     * @param gravity Gravity expressed in units/s^2
     */
    void PhysicsWorld::executeProcessables(const std::vector<std::shared_ptr<Processable>>& processables, float dt, const Vector3<float>& gravity) const {
        ScopeProfiler sp(Profiler::physics(), "execProcessable");

        for (const auto& processable : processables) {
            processable->execute(dt, gravity);
        }
    }

    void PhysicsWorld::createCollisionVisualizer() {
        if (!collisionVisualizer) {
            collisionVisualizer = std::make_unique<CollisionVisualizer>(getCollisionWorld());
        }
    }

    const CollisionVisualizer& PhysicsWorld::getCollisionVisualizer() const {
        if (!collisionVisualizer) {
            throw std::runtime_error("Impossible to get collision visualizer because not created.");
        }

        return *collisionVisualizer;
    }

}
