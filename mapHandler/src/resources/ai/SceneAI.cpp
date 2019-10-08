#include "SceneAI.h"
#include "resources/ai/NavMeshAgentWriter.h"

namespace urchin
{

    SceneAI::SceneAI() :
            aiManager(nullptr)
    {

    }

    void SceneAI::setAIManager(AIManager *aiManager)
    {
        this->aiManager = aiManager;

        if(aiManager)
        {
            aiManager->getNavMeshGenerator()->setNavMeshAgent(navMeshAgent);
        }
    }

    std::shared_ptr<NavMeshAgent> SceneAI::getNavMeshAgent() const
    {
        return navMeshAgent;
    }

    void SceneAI::changeNavMeshAgent(std::shared_ptr<NavMeshAgent> navMeshAgent)
    {
        setNavMeshAgent(std::move(navMeshAgent));
    }

    void SceneAI::loadFrom(const std::shared_ptr<XmlChunk> &chunk, const XmlParser &xmlParser)
    {
        std::shared_ptr<XmlChunk> navMeshAgentChunk = xmlParser.getUniqueChunk(true, NAV_MESH_AGENT_TAG, XmlAttribute(), chunk);

        setNavMeshAgent(NavMeshAgentWriter().loadFrom(navMeshAgentChunk, xmlParser));
    }

    void SceneAI::writeOn(const std::shared_ptr<XmlChunk> &chunk, XmlWriter &xmlWriter) const
    {
        std::shared_ptr<XmlChunk> navMeshAgentChunk = xmlWriter.createChunk(NAV_MESH_AGENT_TAG, XmlAttribute(), chunk);

        NavMeshAgentWriter().writeOn(navMeshAgentChunk, navMeshAgent, xmlWriter);
    }

    void SceneAI::setNavMeshAgent(const std::shared_ptr<NavMeshAgent>& navMeshAgent)
    {
        this->navMeshAgent = navMeshAgent;

        if(aiManager)
        {
            aiManager->getNavMeshGenerator()->setNavMeshAgent(navMeshAgent);
        }
    }
}
