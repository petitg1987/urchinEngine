#include <map/save/ai/NavMeshAgentEntityReaderWriter.h>

namespace urchin {

    std::unique_ptr<NavMeshAgentEntity> NavMeshAgentEntityReaderWriter::load(const UdaChunk* aiElementsListChunk, const UdaParser& udaParser) {
        auto navMeshAgentEntity = std::make_unique<NavMeshAgentEntity>();

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

        navMeshAgentEntity->setNavMeshAgent(std::move(navMeshAgent));

        return navMeshAgentEntity;
    }

    void NavMeshAgentEntityReaderWriter::write(UdaChunk& aiElementsListChunk, const NavMeshAgentEntity& navMeshAgentEntity, UdaWriter& udaWriter) {
        auto& navMeshAgentChunk = udaWriter.createChunk(NAV_MESH_AGENT_TAG, UdaAttribute(), &aiElementsListChunk);

        auto& agentHeightChunk = udaWriter.createChunk(AGENT_HEIGHT_TAG, UdaAttribute(), &navMeshAgentChunk);
        agentHeightChunk.setFloatValue(navMeshAgentEntity.getNavMeshAgent().getAgentHeight());
        auto& agentRadiusChunk = udaWriter.createChunk(AGENT_RADIUS_TAG, UdaAttribute(), &navMeshAgentChunk);
        agentRadiusChunk.setFloatValue(navMeshAgentEntity.getNavMeshAgent().getAgentRadius());
        auto& maxSlopeInRadianChunk = udaWriter.createChunk(MAX_SLOPE_IN_RADIAN_TAG, UdaAttribute(), &navMeshAgentChunk);
        maxSlopeInRadianChunk.setFloatValue(navMeshAgentEntity.getNavMeshAgent().getMaxSlope());
        auto& jumpDistanceChunk = udaWriter.createChunk(JUMP_DISTANCE_TAG, UdaAttribute(), &navMeshAgentChunk);
        jumpDistanceChunk.setFloatValue(navMeshAgentEntity.getNavMeshAgent().getJumpDistance());
    }
}
