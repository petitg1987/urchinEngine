#include <chrono>

#include "PhysicsWorld.h"
#include "processable/raytest/RayTester.h"

namespace urchin
{

	PhysicsWorld::PhysicsWorld() :
			physicsSimulationThread(nullptr),
			physicsSimulationStopper(false),
			timeStep(0),
			maxSubStep(1),
			paused(false),
			bodyManager(new BodyManager()),
			collisionWorld(new CollisionWorld(bodyManager))
	{
		#ifdef _DEBUG
			collisionVisualizer = new CollisionVisualizer(collisionWorld);
		#endif
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

		std::vector<std::shared_ptr<Processable>>::iterator itFind = std::find(processables.begin(), processables.end(), processable);
		if(itFind!=processables.end())
		{
			processables.erase(itFind);
		}
	}

	void PhysicsWorld::rayTest(const Ray<float> &ray, RayTestCallback &rayTestCallback)
	{
		std::lock_guard<std::mutex> lock(mutex);

		std::shared_ptr<RayTester> rayTester = std::make_shared<RayTester>(ray, rayTestCallback);
		rayTester->initialize(this);

		oneShotProcessables.push_back(rayTester);
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
	 * @param maxSubStep Maximum number of sub steps to execute
	 */
	void PhysicsWorld::start(float timeStep, unsigned int maxSubStep)
	{
		if(physicsSimulationThread!=nullptr)
		{
			throw std::runtime_error("Physics thread is already started");
		}

		this->timeStep = timeStep;
		this->maxSubStep = maxSubStep;

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
		double remainingTime = 0.0;
		auto frameStartTime = std::chrono::high_resolution_clock::now();

		while(continueExecution())
		{
			//Compute number of sub steps to execute.
			//Note: rest of division could be used in next steps and interpolate between steps (see http://gafferongames.com/game-physics/fix-your-timestep/)
			unsigned int numSteps = static_cast<unsigned int>((timeStep - remainingTime) / timeStep);
			//Clamp number of sub steps to max sub step to avoid spiral of death (time of simulation increase and need to be executed increasingly).
			unsigned int numStepsClamped = (numSteps > maxSubStep) ? maxSubStep : numSteps;

			for(unsigned int step=0; step<numStepsClamped; ++step)
			{
				processPhysicsUpdate();
			}

			auto frameEndTime = std::chrono::high_resolution_clock::now();
			auto diffTimeMicroSeconds = std::chrono::duration_cast<std::chrono::microseconds>(frameEndTime - frameStartTime).count();
			remainingTime = timeStep - (diffTimeMicroSeconds / 1000000.0);

			if(remainingTime >= 0.0)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(remainingTime * 1000.0)));

				remainingTime = 0.0;
				frameStartTime = std::chrono::high_resolution_clock::now();
			}else
			{
				frameStartTime = frameEndTime;
			}
		}
	}

	/**
	 * @return True if thread execution is not interrupted
	 */
	bool PhysicsWorld::continueExecution()
	{
		return !physicsSimulationStopper.load(std::memory_order_relaxed);
	}

	void PhysicsWorld::processPhysicsUpdate()
	{
		//shared data copy for local thread
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
			float dt = static_cast<float>(timeStep);

			setupProcessables(processables, dt, gravity);

			collisionWorld->process(dt, gravity);

			executeProcessables(processables, dt, gravity);
		}
	}

	/**
	 * @param dt Delta of time between two simulation steps
	 * @param gravity Gravity expressed in units/s^2
	 */
	void PhysicsWorld::setupProcessables(const std::vector<std::shared_ptr<Processable>> &processables, float dt, const Vector3<float> &gravity)
	{
		for(unsigned int i=0; i<processables.size(); ++i)
		{
			processables[i]->setup(dt, gravity);
		}
	}

	/**
	 * @param dt Delta of time between two simulation steps
	 * @param gravity Gravity expressed in units/s^2
	 */
	void PhysicsWorld::executeProcessables(const std::vector<std::shared_ptr<Processable>> &processables, float dt, const Vector3<float> &gravity)
	{
		for(unsigned int i=0; i<processables.size(); ++i)
		{
			processables[i]->execute(dt, gravity);
		}
	}

#ifdef _DEBUG
	const CollisionVisualizer *PhysicsWorld::getCollisionVisualizer() const
	{
		return collisionVisualizer;
	}
#endif
}
