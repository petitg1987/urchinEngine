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

    void SceneAI::loadFrom(const UdaChunk* chunk, const UdaParser& udaParser) {
        auto navMeshAgentChunk = udaParser.getUniqueChunk(true, NAV_MESH_AGENT_TAG, UdaAttribute(), chunk);

        changeNavMeshAgent(NavMeshAgentReaderWriter::loadFrom(navMeshAgentChunk, udaParser));
    }

    void SceneAI::writeOn(UdaChunk& chunk, UdaWriter& udaWriter) const {
        auto& navMeshAgentChunk = udaWriter.createChunk(NAV_MESH_AGENT_TAG, UdaAttribute(), &chunk);

        NavMeshAgentReaderWriter::writeOn(navMeshAgentChunk, getNavMeshAgent(), udaWriter);
    }

}
