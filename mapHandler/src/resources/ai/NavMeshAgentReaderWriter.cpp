#include "NavMeshAgentReaderWriter.h"

namespace urchin {
    std::shared_ptr<NavMeshAgent> NavMeshAgentReaderWriter::loadFrom(const std::shared_ptr<XmlChunk>& navMeshAgentChunk, const XmlParser& xmlParser) {
        std::shared_ptr<XmlChunk> agentHeightChunk = xmlParser.getUniqueChunk(true, AGENT_HEIGHT_TAG, XmlAttribute(), navMeshAgentChunk);
        float agentHeight = agentHeightChunk->getFloatValue();
        std::shared_ptr<XmlChunk> agentRadiusChunk = xmlParser.getUniqueChunk(true, AGENT_RADIUS_TAG, XmlAttribute(), navMeshAgentChunk);
        float agentRadius = agentRadiusChunk->getFloatValue();
        std::shared_ptr<XmlChunk> maxSlopeInRadianChunk = xmlParser.getUniqueChunk(true, MAX_SLOPE_IN_RADIAN_TAG, XmlAttribute(), navMeshAgentChunk);
        float maxSlopeInRadian = maxSlopeInRadianChunk->getFloatValue();
        std::shared_ptr<XmlChunk> jumpDistanceChunk = xmlParser.getUniqueChunk(true, JUMP_DISTANCE_TAG, XmlAttribute(), navMeshAgentChunk);
        float jumpDistance = jumpDistanceChunk->getFloatValue();

        std::shared_ptr<NavMeshAgent> navMeshAgent = std::make_shared<NavMeshAgent>(NavMeshAgent(agentHeight, agentRadius));
        navMeshAgent->setMaxSlope(maxSlopeInRadian);
        navMeshAgent->setJumpDistance(jumpDistance);

        return navMeshAgent;
    }

    void NavMeshAgentReaderWriter::writeOn(const std::shared_ptr<XmlChunk>& navMeshAgentChunk, const std::shared_ptr<const NavMeshAgent>& navMeshAgent, XmlWriter& xmlWriter) {
        std::shared_ptr<XmlChunk> agentHeightChunk = xmlWriter.createChunk(AGENT_HEIGHT_TAG, XmlAttribute(), navMeshAgentChunk);
        agentHeightChunk->setFloatValue(navMeshAgent->getAgentHeight());
        std::shared_ptr<XmlChunk> agentRadiusChunk = xmlWriter.createChunk(AGENT_RADIUS_TAG, XmlAttribute(), navMeshAgentChunk);
        agentRadiusChunk->setFloatValue(navMeshAgent->getAgentRadius());
        std::shared_ptr<XmlChunk> maxSlopeInRadianChunk = xmlWriter.createChunk(MAX_SLOPE_IN_RADIAN_TAG, XmlAttribute(), navMeshAgentChunk);
        maxSlopeInRadianChunk->setFloatValue(navMeshAgent->getMaxSlope());
        std::shared_ptr<XmlChunk> jumpDistanceChunk = xmlWriter.createChunk(JUMP_DISTANCE_TAG, XmlAttribute(), navMeshAgentChunk);
        jumpDistanceChunk->setFloatValue(navMeshAgent->getJumpDistance());
    }
}
