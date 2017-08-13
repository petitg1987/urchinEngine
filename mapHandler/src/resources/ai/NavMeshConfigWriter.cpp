#include "NavMeshConfigWriter.h"

namespace urchin
{
    std::shared_ptr<NavMeshConfig> NavMeshConfigWriter::loadFrom(std::shared_ptr<XmlChunk> navMeshConfigChunk, const XmlParser &xmlParser) const
    {
        std::shared_ptr<XmlChunk> maxSlopeInRadianChunk = xmlParser.getUniqueChunk(true, MAX_SLOPE_IN_RADIAN_TAG, XmlAttribute(), navMeshConfigChunk);
        float maxSlopeInRadian = maxSlopeInRadianChunk->getFloatValue();

        std::shared_ptr<XmlChunk> agentChunk = xmlParser.getUniqueChunk(true, AGENT_TAG, XmlAttribute(), navMeshConfigChunk);
        std::shared_ptr<XmlChunk> agentHeightChunk = xmlParser.getUniqueChunk(true, AGENT_HEIGHT_TAG, XmlAttribute(), agentChunk);
        float agentHeight = agentHeightChunk->getFloatValue();
        std::shared_ptr<XmlChunk> agentRadiusChunk = xmlParser.getUniqueChunk(true, AGENT_RADIUS_TAG, XmlAttribute(), agentChunk);
        float agentRadius = agentRadiusChunk->getFloatValue();

        std::shared_ptr<NavMeshConfig> navMeshConfig = std::make_shared<NavMeshConfig>(NavMeshAgent(agentHeight, agentRadius));
        navMeshConfig->setMaxSlope(maxSlopeInRadian);

        return navMeshConfig;
    }

    void NavMeshConfigWriter::writeOn(std::shared_ptr<XmlChunk> navMeshConfigChunk, std::shared_ptr<const NavMeshConfig> navMeshConfig, XmlWriter &xmlWriter) const
    {
        std::shared_ptr<XmlChunk> maxSlopeInRadianChunk = xmlWriter.createChunk(MAX_SLOPE_IN_RADIAN_TAG, XmlAttribute(), navMeshConfigChunk);
        maxSlopeInRadianChunk->setFloatValue(navMeshConfig->getMaxSlope());

        std::shared_ptr<XmlChunk> agentChunk = xmlWriter.createChunk(AGENT_TAG, XmlAttribute(), navMeshConfigChunk);
        std::shared_ptr<XmlChunk> agentHeightChunk = xmlWriter.createChunk(AGENT_HEIGHT_TAG, XmlAttribute(), agentChunk);
        agentHeightChunk->setFloatValue(navMeshConfig->getAgent().getAgentHeight());
        std::shared_ptr<XmlChunk> agentRadiusChunk = xmlWriter.createChunk(AGENT_RADIUS_TAG, XmlAttribute(), agentChunk);
        agentRadiusChunk->setFloatValue(navMeshConfig->getAgent().getAgentRadius());
    }
}
