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

    void SceneAI::loadFrom(const XmlChunk* chunk, const DataParser& dataParser) {
        auto navMeshAgentChunk = dataParser.getUniqueChunk(true, NAV_MESH_AGENT_TAG, DataAttribute(), chunk);

        changeNavMeshAgent(NavMeshAgentReaderWriter::loadFrom(navMeshAgentChunk.get(), dataParser));
    }

    void SceneAI::writeOn(XmlChunk* chunk, XmlWriter& xmlWriter) const {
        auto navMeshAgentChunk = xmlWriter.createChunk(NAV_MESH_AGENT_TAG, DataAttribute(), chunk);

        NavMeshAgentReaderWriter::writeOn(navMeshAgentChunk.get(), getNavMeshAgent(), xmlWriter);
    }

}
