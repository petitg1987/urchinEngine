#include <chrono>
#include <algorithm>

#include "PhysicsWorld.h"
#include "processable/raytest/RayTester.h"

#define DEFAULT_GRAVITY Vector3<float>(0.0f, -9.81f, 0.0f)

namespace urchin
{

	PhysicsWorld::PhysicsWorld() :
			physicsSimulationThread(nullptr),
			physicsSimulationStopper(false),
			gravity(DEFAULT_GRAVITY),
			timeStep(0.0f),
			paused(true),
			bodyManager(new BodyManager()),
			collisionWorld(new CollisionWorld(bodyManager))
	{
		#ifdef _DEBUG
			collisionVisualizer = new CollisionVisualizer(collisionWorld);
        #endif

		NumericalCheck::instance()->perform();
	}

	PhysicsWorld::~PhysicsWorld()
	{
		if(physicsSimulationThread!=nullptr)
		{
			interrupt();
			physicsSimulationThread->join();

			delete physicsSimulationThread;
		}

		processables.clear();
		oneShotProcessables.clear();

		#ifdef _DEBUG
			delete collisionVisualizer;
		#endif

		delete collisionWorld;
		delete bodyManager;

		Profiler::getInstance("physics")->log();
	}

	BodyManager *PhysicsWorld::getBodyManager() const
	{
		return bodyManager;
	}

	CollisionWorld *PhysicsWorld::getCollisionWorld() const
	{
		return collisionWorld;
	}

	void PhysicsWorld::addBody(AbstractBody *body)
	{
		if(body!=nullptr)
		{
			bodyManager->addBody(body);
		}
	}

	void PhysicsWorld::removeBody(AbstractBody *body)
	{
		if(body!=nullptr)
		{
			bodyManager->removeBody(body);
		}
	}

	void PhysicsWorld::addProcessable(std::shared_ptr<Processable> processable)
	{
		std::lock_guard<std::mutex> lock(mutex);

		processable->initialize(this);

		processables.push_back(processable);
	}

	void PhysicsWorld::removeProcessable(std::shared_ptr<Processable> processable)
	{
		std::lock_guard<std::mutex> lock(mutex);

		auto itFind = std::find(processables.begin(), processables.end(), processable);
		if(itFind!=processables.end())
		{
			processables.erase(itFind);
		}
	}

	std::shared_ptr<const RayTestResult> PhysicsWorld::rayTest(const Ray<float> &ray)
	{
		std::lock_guard<std::mutex> lock(mutex);

		std::shared_ptr<RayTester> rayTester = std::make_shared<RayTester>(ray);
		rayTester->initialize(this);

		oneShotProcessables.push_back(rayTester);

		return rayTester->getRayTestResult();
	}

	/**
	 * @param gravity Gravity expressed in units/s^2
	 */
	void PhysicsWorld::setGravity(const Vector3<float> &gravity)
	{
		std::lock_guard<std::mutex> lock(mutex);

		this->gravity = gravity;
	}

	/**
	 * @return gravity expressed in units/s^2
	 */
	Vector3<float> PhysicsWorld::getGravity() const
	{
		std::lock_guard<std::mutex> lock(mutex);

		return gravity;
	}

	/**
	 * Launch the physics simulation in new thread
	 * @param timeStep Frequency updates expressed in second
	 */
	void PhysicsWorld::start(float timeStep, bool startPaused)
	{
		if(physicsSimulationThread!=nullptr)
		{
			throw std::runtime_error("Physics thread is already started");
		}

		this->timeStep = timeStep;
		this->paused = startPaused;

		physicsSimulationThread = new std::thread(&PhysicsWorld::startPhysicsUpdate, this);
	}

	void PhysicsWorld::pause()
	{
		std::lock_guard<std::mutex> lock(mutex);

		paused = true;
	}

	void PhysicsWorld::play()
	{
		std::lock_guard<std::mutex> lock(mutex);

		paused = false;
	}

	bool PhysicsWorld::isPaused() const
	{
		std::lock_guard<std::mutex> lock(mutex);

		return paused;
	}

	/**
	 * Interrupt the thread
	 */
	void PhysicsWorld::interrupt()
	{
		physicsSimulationStopper.store(true, std::memory_order_relaxed);
	}

	void PhysicsWorld::startPhysicsUpdate()
	{
		try
		{
			float remainingTime = 0.0f;
            float maxAdditionalTimeStep = timeStep * 0.5f;
			auto frameStartTime = std::chrono::high_resolution_clock::now();

			while (continueExecution())
			{
                float additionalTimeStep = std::abs(remainingTime);
                if(additionalTimeStep > maxAdditionalTimeStep)
                {
					//Cannot process physics update with 'additionalTimeStep'. Value is too big and can lead to physics errors.
					//Use 'maxAdditionalTimeStep' which lead to slow-down of the physics.
                    additionalTimeStep = maxAdditionalTimeStep;
                }

                processPhysicsUpdate(timeStep + additionalTimeStep);

				auto frameEndTime = std::chrono::high_resolution_clock::now();
				auto diffTimeMicroSeconds = std::chrono::duration_cast<std::chrono::microseconds>(frameEndTime - frameStartTime).count();
				remainingTime = (timeStep + additionalTimeStep) - (diffTimeMicroSeconds / 1000000.0);

				if (remainingTime >= 0.0f)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(remainingTime * 1000.0f)));

					remainingTime = 0.0f;
					frameStartTime = std::chrono::high_resolution_clock::now();
				} else
				{
					frameStartTime = frameEndTime;
				}
			}
		}catch(std::exception &e)
		{
			Logger::logger().logError("Error cause physics thread crash: " + std::string(e.what()));
            throw e;
		}
	}

	/**
	 * @return True if thread execution is not interrupted
	 */
	bool PhysicsWorld::continueExecution()
	{
		return !physicsSimulationStopper.load(std::memory_order_relaxed);
	}

	void PhysicsWorld::processPhysicsUpdate(float frameTimeStep)
	{
		ScopeProfiler profiler("physics", "processPhysicsUpdate");

		//copy for local thread
		bool paused;
		Vector3<float> gravity;
		std::vector<std::shared_ptr<Processable>> processables;
		{
			std::lock_guard<std::mutex> lock(mutex);

			paused = this->paused;

			gravity = this->gravity;

			processables = this->processables;
			processables.insert(processables.end(), oneShotProcessables.begin(), oneShotProcessables.end());
			oneShotProcessables.clear();
		}

		//physics execution
		if(!paused)
		{
			setupProcessables(processables, frameTimeStep, gravity);

			collisionWorld->process(frameTimeStep, gravity);

			executeProcessables(processables, frameTimeStep, gravity);
		}
	}

	/**
	 * @param dt Delta of time between two simulation steps
	 * @param gravity Gravity expressed in units/s^2
	 */
	void PhysicsWorld::setupProcessables(const std::vector<std::shared_ptr<Processable>> &processables, float dt, const Vector3<float> &gravity)
	{
		ScopeProfiler profiler("physics", "setupProcessables");

		for (const auto &processable : processables)
		{
			processable->setup(dt, gravity);
		}
	}

	/**
	 * @param dt Delta of time between two simulation steps
	 * @param gravity Gravity expressed in units/s^2
	 */
	void PhysicsWorld::executeProcessables(const std::vector<std::shared_ptr<Processable>> &processables, float dt, const Vector3<float> &gravity)
	{
		ScopeProfiler profiler("physics", "executeProcessables");

		for (const auto &processable : processables)
		{
			processable->execute(dt, gravity);
		}
	}

#ifdef _DEBUG
	const CollisionVisualizer *PhysicsWorld::getCollisionVisualizer() const
	{
		return collisionVisualizer;
	}
#endif
}
