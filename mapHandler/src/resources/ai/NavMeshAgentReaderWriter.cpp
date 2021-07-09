#include "NavMeshAgentReaderWriter.h"

namespace urchin {
    std::shared_ptr<NavMeshAgent> NavMeshAgentReaderWriter::loadFrom(const XmlChunk* navMeshAgentChunk, const DataParser& dataParser) {
        auto agentHeightChunk = dataParser.getUniqueChunk(true, AGENT_HEIGHT_TAG, DataAttribute(), navMeshAgentChunk);
        float agentHeight = agentHeightChunk->getFloatValue();
        auto agentRadiusChunk = dataParser.getUniqueChunk(true, AGENT_RADIUS_TAG, DataAttribute(), navMeshAgentChunk);
        float agentRadius = agentRadiusChunk->getFloatValue();
        auto maxSlopeInRadianChunk = dataParser.getUniqueChunk(true, MAX_SLOPE_IN_RADIAN_TAG, DataAttribute(), navMeshAgentChunk);
        float maxSlopeInRadian = maxSlopeInRadianChunk->getFloatValue();
        auto jumpDistanceChunk = dataParser.getUniqueChunk(true, JUMP_DISTANCE_TAG, DataAttribute(), navMeshAgentChunk);
        float jumpDistance = jumpDistanceChunk->getFloatValue();

        std::shared_ptr<NavMeshAgent> navMeshAgent = std::make_shared<NavMeshAgent>(NavMeshAgent(agentHeight, agentRadius));
        navMeshAgent->setMaxSlope(maxSlopeInRadian);
        navMeshAgent->setJumpDistance(jumpDistance);

        return navMeshAgent;
    }

    void NavMeshAgentReaderWriter::writeOn(XmlChunk* navMeshAgentChunk, const std::shared_ptr<const NavMeshAgent>& navMeshAgent, XmlWriter& xmlWriter) {
        auto agentHeightChunk = xmlWriter.createChunk(AGENT_HEIGHT_TAG, DataAttribute(), navMeshAgentChunk);
        agentHeightChunk->setFloatValue(navMeshAgent->getAgentHeight());
        auto agentRadiusChunk = xmlWriter.createChunk(AGENT_RADIUS_TAG, DataAttribute(), navMeshAgentChunk);
        agentRadiusChunk->setFloatValue(navMeshAgent->getAgentRadius());
        auto maxSlopeInRadianChunk = xmlWriter.createChunk(MAX_SLOPE_IN_RADIAN_TAG, DataAttribute(), navMeshAgentChunk);
        maxSlopeInRadianChunk->setFloatValue(navMeshAgent->getMaxSlope());
        auto jumpDistanceChunk = xmlWriter.createChunk(JUMP_DISTANCE_TAG, DataAttribute(), navMeshAgentChunk);
        jumpDistanceChunk->setFloatValue(navMeshAgent->getJumpDistance());
    }
}
