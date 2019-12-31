#include "SceneAI.h"
#include "resources/ai/NavMeshAgentWriter.h"

namespace urchin
{

    SceneAI::SceneAI(AIManager *aiManager) :
            aiManager(aiManager)
    {

    }

    std::shared_ptr<NavMeshAgent> SceneAI::getNavMeshAgent() const
    {
        return aiManager->getNavMeshGenerator()->getNavMeshAgent();
    }

    void SceneAI::changeNavMeshAgent(const std::shared_ptr<NavMeshAgent> &navMeshAgent)
    {
        aiManager->getNavMeshGenerator()->setNavMeshAgent(navMeshAgent);
    }

    void SceneAI::loadFrom(const std::shared_ptr<XmlChunk> &chunk, const XmlParser &xmlParser)
    {
        std::shared_ptr<XmlChunk> navMeshAgentChunk = xmlParser.getUniqueChunk(true, NAV_MESH_AGENT_TAG, XmlAttribute(), chunk);

        changeNavMeshAgent(NavMeshAgentWriter().loadFrom(navMeshAgentChunk, xmlParser));
    }

    void SceneAI::writeOn(const std::shared_ptr<XmlChunk> &chunk, XmlWriter &xmlWriter) const
    {
        std::shared_ptr<XmlChunk> navMeshAgentChunk = xmlWriter.createChunk(NAV_MESH_AGENT_TAG, XmlAttribute(), chunk);

        NavMeshAgentWriter().writeOn(navMeshAgentChunk, getNavMeshAgent(), xmlWriter);
    }

}
