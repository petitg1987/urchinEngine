#ifndef ENGINE_INTEGRATEVELOCITYMANAGER_H
#define ENGINE_INTEGRATEVELOCITYMANAGER_H

#include "UrchinCommon.h"
#include <vector>

#include "body/BodyManager.h"
#include "collision/ManifoldResult.h"

namespace urchin
{

	/**
	* Manager allowing to perform integration on bodies velocity
	*/
	class IntegrateVelocityManager
	{
		public:
			IntegrateVelocityManager(const BodyManager *);
			~IntegrateVelocityManager();

			void integrateVelocity(float, std::vector<ManifoldResult> &, const Vector3<float> &);

		private:
			void applyGravityForce(const Vector3<float> &);
			void applyRollingFrictionResistanceForce(float , std::vector<ManifoldResult> &);

			const BodyManager *bodyManager;
	};

}

#endif
