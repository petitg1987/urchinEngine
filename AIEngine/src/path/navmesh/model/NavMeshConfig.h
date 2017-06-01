#ifndef ENGINE_NAVMESHCONFIG_H
#define ENGINE_NAVMESHCONFIG_H

#include "UrchinCommon.h"

#include "path/navmesh/model/NavMeshAgent.h"

namespace urchin
{

	class NavMeshConfig
	{
		public:
			NavMeshConfig(const NavMeshAgent &);

			const NavMeshAgent &getAgent() const;

			void setMaxSlope(float);
			float getMaxSlope() const;

		private:
            NavMeshAgent navMeshAgent;
			float maxSlopeInRadian;
	};

}

#endif
