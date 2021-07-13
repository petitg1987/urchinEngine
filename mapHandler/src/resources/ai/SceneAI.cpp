#include "SceneAI.h"
#include <resources/ai/NavMeshAgentReaderWriter.h>

namespace urchin {

    SceneAI::SceneAI(AIManager* aiManager) :
            aiManager(aiManager) {

    }

    const NavMeshAgent* SceneAI::getNavMeshAgent() const {
        return aiManager->getNavMeshGenerator().getNavMeshAgent();
    }

    void SceneAI::changeNavMeshAgent(std::unique_ptr<NavMeshAgent> navMeshAgent) {
        aiManager->getNavMeshGenerator().setNavMeshAgent(std::move(navMeshAgent));
    }

    void SceneAI::loadFrom(const DataChunk* chunk, const DataParser& dataParser) {
        auto navMeshAgentChunk = dataParser.getUniqueChunk(true, NAV_MESH_AGENT_TAG, UdaAttribute(), chunk);

        changeNavMeshAgent(NavMeshAgentReaderWriter::loadFrom(navMeshAgentChunk, dataParser));
    }

    void SceneAI::writeOn(DataChunk& chunk, UdaWriter& udaWriter) const {
        auto& navMeshAgentChunk = udaWriter.createChunk(NAV_MESH_AGENT_TAG, UdaAttribute(), &chunk);

        NavMeshAgentReaderWriter::writeOn(navMeshAgentChunk, getNavMeshAgent(), udaWriter);
    }

}
