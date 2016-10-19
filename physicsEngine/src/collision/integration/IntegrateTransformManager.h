#ifndef ENGINE_INTEGRATETRANSFORMMANAGER_H
#define ENGINE_INTEGRATETRANSFORMMANAGER_H

#include "body/BodyManager.h"
#include "body/work/WorkRigidBody.h"
#include "collision/broadphase/BroadPhaseManager.h"
#include "collision/narrowphase/NarrowPhaseManager.h"

namespace urchin
{

	/**
	* Manager allowing to perform integration on bodies transformation
	*/
	class IntegrateTransformManager
	{
		public:
			IntegrateTransformManager(const BodyManager *, const BroadPhaseManager *, const NarrowPhaseManager *);
			~IntegrateTransformManager();

			void integrateTransform(float);

		private:
			PhysicsTransform computeNewIntegrateTransform(float, const WorkRigidBody *) const;
			void handleContinuousCollision(WorkRigidBody *, const PhysicsTransform &, const PhysicsTransform &);

			const BodyManager *bodyManager;
			const BroadPhaseManager *broadPhaseManager;
			const NarrowPhaseManager *narrowPhaseManager;
	};

}

#endif
