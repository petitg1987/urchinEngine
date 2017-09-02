#ifndef URCHINENGINE_INTEGRATETRANSFORMMANAGER_H
#define URCHINENGINE_INTEGRATETRANSFORMMANAGER_H

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

			void integrateTransform(float);

		private:
			void handleContinuousCollision(WorkRigidBody *, const PhysicsTransform &, const PhysicsTransform &, float);

			const BodyManager *bodyManager;
			const BroadPhaseManager *broadPhaseManager;
			const NarrowPhaseManager *narrowPhaseManager;
	};

}

#endif
