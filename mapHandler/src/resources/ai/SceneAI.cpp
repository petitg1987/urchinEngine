#include "SceneAI.h"
#include <resources/ai/NavMeshAgentReaderWriter.h>

namespace urchin {

    SceneAI::SceneAI(AIManager* aiManager) :
            aiManager(aiManager) {

    }

    std::shared_ptr<NavMeshAgent> SceneAI::getNavMeshAgent() const {
        return aiManager->getNavMeshGenerator()->getNavMeshAgent();
    }

    void SceneAI::changeNavMeshAgent(const std::shared_ptr<NavMeshAgent>& navMeshAgent) {
        aiManager->getNavMeshGenerator()->setNavMeshAgent(navMeshAgent);
    }

    void SceneAI::loadFrom(const XmlChunk* chunk, const XmlParser& xmlParser) {
        auto navMeshAgentChunk = xmlParser.getUniqueChunk(true, NAV_MESH_AGENT_TAG, XmlAttribute(), chunk);

        changeNavMeshAgent(NavMeshAgentReaderWriter::loadFrom(navMeshAgentChunk.get(), xmlParser));
    }

    void SceneAI::writeOn(XmlChunk* chunk, XmlWriter& xmlWriter) const {
        auto navMeshAgentChunk = xmlWriter.createChunk(NAV_MESH_AGENT_TAG, XmlAttribute(), chunk);

        NavMeshAgentReaderWriter::writeOn(navMeshAgentChunk.get(), getNavMeshAgent(), xmlWriter);
    }

}
