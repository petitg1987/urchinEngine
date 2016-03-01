#ifndef ENGINE_PHYSICSWORLD_H
#define ENGINE_PHYSICSWORLD_H

#include <vector>
#include <atomic>
#include <thread>
#include "UrchinCommon.h"

#include "body/AbstractBody.h"
#include "body/BodyManager.h"
#include "collision/CollisionWorld.h"
#include "processable/ProcessableInterface.h"
#include "visualizer/CollisionVisualizer.h"

namespace urchin
{

	class PhysicsWorld
	{
		public:
			PhysicsWorld();
			~PhysicsWorld();

			BodyManager *getBodyManager() const;
			CollisionWorld *getCollisionWorld() const;

			void addBody(AbstractBody *);
			void removeBody(AbstractBody *);

			void addProcessable(ProcessableInterface *);
			void removeProcessable(ProcessableInterface *);

			void setGravity(const Vector3<float> &);
			Vector3<float> getGravity() const;

			void start(float, unsigned int maxStep = 1);
			void pause();
			void play();
			bool isPaused() const;
			void interrupt();

			#ifdef _DEBUG
				const CollisionVisualizer *getCollisionVisualizer() const;
			#endif

		private:
			void startPhysicsUpdate();
			bool continueExecution();
			void processPhysicsUpdate();

			void preProcess(float, const Vector3<float> &);
			void postProcess(float, const Vector3<float> &);

			std::thread *physicsSimulationThread;
			std::atomic_bool physicsSimulationStopper;

			mutable std::mutex mutex;
			Vector3<float> gravity;
			float timeStep;
			unsigned int maxSubStep;
			bool paused;

			BodyManager *bodyManager;
			CollisionWorld *collisionWorld;
			std::vector<ProcessableInterface *> processables;
			#ifdef _DEBUG
				CollisionVisualizer *collisionVisualizer;
			#endif
	};

}

#endif
