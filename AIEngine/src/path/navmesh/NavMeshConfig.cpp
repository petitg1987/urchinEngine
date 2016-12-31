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

	Cylinder<float> NavMeshConfig::getAgent() const
	{
		return Cylinder<float>(agentRadius, agentHeight, CylinderShape<float>::CYLINDER_Y, Point3<float>(0.0, 0.0, 0.0), Quaternion<float>());
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
