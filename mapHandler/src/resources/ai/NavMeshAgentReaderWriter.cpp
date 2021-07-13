#include "NavMeshAgentReaderWriter.h"

namespace urchin {

    std::unique_ptr<NavMeshAgent> NavMeshAgentReaderWriter::loadFrom(const DataChunk* navMeshAgentChunk, const DataParser& dataParser) {
        auto agentHeightChunk = dataParser.getUniqueChunk(true, AGENT_HEIGHT_TAG, DataAttribute(), navMeshAgentChunk);
        float agentHeight = agentHeightChunk->getFloatValue();
        auto agentRadiusChunk = dataParser.getUniqueChunk(true, AGENT_RADIUS_TAG, DataAttribute(), navMeshAgentChunk);
        float agentRadius = agentRadiusChunk->getFloatValue();
        auto maxSlopeInRadianChunk = dataParser.getUniqueChunk(true, MAX_SLOPE_IN_RADIAN_TAG, DataAttribute(), navMeshAgentChunk);
        float maxSlopeInRadian = maxSlopeInRadianChunk->getFloatValue();
        auto jumpDistanceChunk = dataParser.getUniqueChunk(true, JUMP_DISTANCE_TAG, DataAttribute(), navMeshAgentChunk);
        float jumpDistance = jumpDistanceChunk->getFloatValue();

        std::unique_ptr<NavMeshAgent> navMeshAgent = std::make_unique<NavMeshAgent>(NavMeshAgent(agentHeight, agentRadius));
        navMeshAgent->setMaxSlope(maxSlopeInRadian);
        navMeshAgent->setJumpDistance(jumpDistance);

        return navMeshAgent;
    }

    void NavMeshAgentReaderWriter::writeOn(DataChunk& navMeshAgentChunk, const NavMeshAgent* navMeshAgent, DataWriter& dataWriter) {
        auto& agentHeightChunk = dataWriter.createChunk(AGENT_HEIGHT_TAG, DataAttribute(), &navMeshAgentChunk);
        agentHeightChunk.setFloatValue(navMeshAgent->getAgentHeight());
        auto& agentRadiusChunk = dataWriter.createChunk(AGENT_RADIUS_TAG, DataAttribute(), &navMeshAgentChunk);
        agentRadiusChunk.setFloatValue(navMeshAgent->getAgentRadius());
        auto& maxSlopeInRadianChunk = dataWriter.createChunk(MAX_SLOPE_IN_RADIAN_TAG, DataAttribute(), &navMeshAgentChunk);
        maxSlopeInRadianChunk.setFloatValue(navMeshAgent->getMaxSlope());
        auto& jumpDistanceChunk = dataWriter.createChunk(JUMP_DISTANCE_TAG, DataAttribute(), &navMeshAgentChunk);
        jumpDistanceChunk.setFloatValue(navMeshAgent->getJumpDistance());
    }
}
