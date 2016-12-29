#include "NavMeshConfig.h"

#define DEFAULT_MAX_SLOPE 3.14159f/4.0f //45 degrees

namespace urchin
{

	NavMeshConfig::NavMeshConfig(float agentHeight, float agentRadius) :
		agentHeight(agentHeight),
		agentRadius(agentRadius),
		maxSlopeInRadian(DEFAULT_MAX_SLOPE)
	{

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
