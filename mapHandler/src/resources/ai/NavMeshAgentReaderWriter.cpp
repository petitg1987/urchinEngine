#include "NavMeshAgentReaderWriter.h"

namespace urchin {

    std::unique_ptr<NavMeshAgent> NavMeshAgentReaderWriter::loadFrom(const DataChunk* navMeshAgentChunk, const DataParser& dataParser) {
        auto agentHeightChunk = dataParser.getUniqueChunk(true, AGENT_HEIGHT_TAG, UdaAttribute(), navMeshAgentChunk);
        float agentHeight = agentHeightChunk->getFloatValue();
        auto agentRadiusChunk = dataParser.getUniqueChunk(true, AGENT_RADIUS_TAG, UdaAttribute(), navMeshAgentChunk);
        float agentRadius = agentRadiusChunk->getFloatValue();
        auto maxSlopeInRadianChunk = dataParser.getUniqueChunk(true, MAX_SLOPE_IN_RADIAN_TAG, UdaAttribute(), navMeshAgentChunk);
        float maxSlopeInRadian = maxSlopeInRadianChunk->getFloatValue();
        auto jumpDistanceChunk = dataParser.getUniqueChunk(true, JUMP_DISTANCE_TAG, UdaAttribute(), navMeshAgentChunk);
        float jumpDistance = jumpDistanceChunk->getFloatValue();

        std::unique_ptr<NavMeshAgent> navMeshAgent = std::make_unique<NavMeshAgent>(NavMeshAgent(agentHeight, agentRadius));
        navMeshAgent->setMaxSlope(maxSlopeInRadian);
        navMeshAgent->setJumpDistance(jumpDistance);

        return navMeshAgent;
    }

    void NavMeshAgentReaderWriter::writeOn(DataChunk& navMeshAgentChunk, const NavMeshAgent* navMeshAgent, UdaWriter& udaWriter) {
        auto& agentHeightChunk = udaWriter.createChunk(AGENT_HEIGHT_TAG, UdaAttribute(), &navMeshAgentChunk);
        agentHeightChunk.setFloatValue(navMeshAgent->getAgentHeight());
        auto& agentRadiusChunk = udaWriter.createChunk(AGENT_RADIUS_TAG, UdaAttribute(), &navMeshAgentChunk);
        agentRadiusChunk.setFloatValue(navMeshAgent->getAgentRadius());
        auto& maxSlopeInRadianChunk = udaWriter.createChunk(MAX_SLOPE_IN_RADIAN_TAG, UdaAttribute(), &navMeshAgentChunk);
        maxSlopeInRadianChunk.setFloatValue(navMeshAgent->getMaxSlope());
        auto& jumpDistanceChunk = udaWriter.createChunk(JUMP_DISTANCE_TAG, UdaAttribute(), &navMeshAgentChunk);
        jumpDistanceChunk.setFloatValue(navMeshAgent->getJumpDistance());
    }
}
