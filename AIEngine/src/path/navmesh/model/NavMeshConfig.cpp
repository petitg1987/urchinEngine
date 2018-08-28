#include "NavMeshConfig.h"

#define DEFAULT_MAX_SLOPE (PI_VALUE/4.0f) //45 degrees

namespace urchin
{

	NavMeshConfig::NavMeshConfig(const NavMeshAgent &navMeshAgent) :
			navMeshAgent(navMeshAgent),
		    maxSlopeInRadian(DEFAULT_MAX_SLOPE)
	{

	}

	const NavMeshAgent &NavMeshConfig::getAgent() const
	{
		return navMeshAgent;
	}

	void NavMeshConfig::setMaxSlope(float maxSlopeInRadian)
	{
		this->maxSlopeInRadian = maxSlopeInRadian;
	}

	float NavMeshConfig::getMaxSlope() const
	{
		return maxSlopeInRadian;
	}

}
