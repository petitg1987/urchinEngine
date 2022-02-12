#include <map/save/ai/NavMeshAgentReaderWriter.h>

namespace urchin {

    std::unique_ptr<NavMeshAgent> NavMeshAgentReaderWriter::loadNavMeshAgent(const UdaChunk* aiElementsListChunk, const UdaParser& udaParser) {
        auto navMeshAgentChunk = udaParser.getFirstChunk(true, NAV_MESH_AGENT_TAG, UdaAttribute(), aiElementsListChunk);

        auto agentHeightChunk = udaParser.getFirstChunk(true, AGENT_HEIGHT_TAG, UdaAttribute(), navMeshAgentChunk);
        float agentHeight = agentHeightChunk->getFloatValue();
        auto agentRadiusChunk = udaParser.getFirstChunk(true, AGENT_RADIUS_TAG, UdaAttribute(), navMeshAgentChunk);
        float agentRadius = agentRadiusChunk->getFloatValue();
        auto maxSlopeInRadianChunk = udaParser.getFirstChunk(true, MAX_SLOPE_IN_RADIAN_TAG, UdaAttribute(), navMeshAgentChunk);
        float maxSlopeInRadian = maxSlopeInRadianChunk->getFloatValue();
        auto jumpDistanceChunk = udaParser.getFirstChunk(true, JUMP_DISTANCE_TAG, UdaAttribute(), navMeshAgentChunk);
        float jumpDistance = jumpDistanceChunk->getFloatValue();

        auto navMeshAgent = std::make_unique<NavMeshAgent>(NavMeshAgent(agentHeight, agentRadius));
        navMeshAgent->setMaxSlope(maxSlopeInRadian);
        navMeshAgent->setJumpDistance(jumpDistance);

        return navMeshAgent;
    }

    void NavMeshAgentReaderWriter::writeNavMeshAgent(UdaChunk& aiElementsListChunk, const NavMeshAgent* navMeshAgent, UdaWriter& udaWriter) {
        auto& navMeshAgentChunk = udaWriter.createChunk(NAV_MESH_AGENT_TAG, UdaAttribute(), &aiElementsListChunk);

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
