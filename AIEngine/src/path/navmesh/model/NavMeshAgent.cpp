#include "NavMeshAgent.h"

#define DEFAULT_AGENT_HEIGHT 1.8
#define DEFAULT_AGENT_RADIUS 0.25

namespace urchin {

    NavMeshAgent::NavMeshAgent() :
            agentHeight(DEFAULT_AGENT_HEIGHT),
            agentRadius(DEFAULT_AGENT_RADIUS)
    {

    }

    NavMeshAgent::NavMeshAgent(float agentHeight, float agentRadius) :
            agentHeight(agentHeight),
            agentRadius(agentRadius)
    {

    }

    float NavMeshAgent::getAgentHeight() const
    {
        return agentHeight;
    }

    float NavMeshAgent::getAgentRadius() const
    {
        return agentRadius;
    }

    CylinderShape<float> NavMeshAgent::getAgentCylinder() const
    {
        return CylinderShape<float>(agentRadius, agentHeight, CylinderShape<float>::CYLINDER_Y);
    }

    BoxShape<float> NavMeshAgent::getAgentBox() const
    {
        return BoxShape<float>(Vector3<float>(agentRadius, agentHeight / 2.0f, agentRadius));
    }

    float NavMeshAgent::computeExpandDistance(const Vector3<float> &normal) const
    {
        Point3<float> supportPoint = getAgentBox().getSupportPoint(-normal);
        return supportPoint.toVector().dotProduct(-normal);
    }

}