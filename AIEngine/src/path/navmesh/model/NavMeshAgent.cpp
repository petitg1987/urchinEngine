#include "NavMeshAgent.h"

namespace urchin {

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