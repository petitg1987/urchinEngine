#include <path/navmesh/model/output/NavMeshAgent.h>

namespace urchin {

    NavMeshAgent::NavMeshAgent() :
            agentHeight(DEFAULT_AGENT_HEIGHT),
            agentRadius(DEFAULT_AGENT_RADIUS),
            maxSlopeInRadian(DEFAULT_MAX_SLOPE),
            jumpDistance(DEFAULT_JUMP_DISTANCE) {

    }

    NavMeshAgent::NavMeshAgent(float agentHeight, float agentRadius) :
            agentHeight(agentHeight),
            agentRadius(agentRadius),
            maxSlopeInRadian(DEFAULT_MAX_SLOPE),
            jumpDistance(DEFAULT_JUMP_DISTANCE) {

    }

    float NavMeshAgent::getAgentHeight() const {
        return agentHeight;
    }

    float NavMeshAgent::getAgentRadius() const {
        return agentRadius;
    }

    void NavMeshAgent::setMaxSlope(float maxSlopeInRadian) {
        this->maxSlopeInRadian = maxSlopeInRadian;
    }

    float NavMeshAgent::getMaxSlope() const {
        return maxSlopeInRadian;
    }

    void NavMeshAgent::setJumpDistance(float jumpDistance) {
        this->jumpDistance = jumpDistance;
    }

    float NavMeshAgent::getJumpDistance() const {
        return jumpDistance;
    }

    CylinderShape<float> NavMeshAgent::getAgentCylinder() const {
        return CylinderShape<float>(agentRadius, agentHeight, CylinderShape<float>::CYLINDER_Y);
    }

    BoxShape<float> NavMeshAgent::getAgentBox() const {
        return BoxShape<float>(Vector3<float>(agentRadius, agentHeight / 2.0f, agentRadius));
    }

    float NavMeshAgent::computeExpandDistance(const Vector3<float>& normal) const {
        Point3<float> supportPoint = getAgentBox().getSupportPoint(-normal);
        return supportPoint.toVector().dotProduct(-normal);
    }

}