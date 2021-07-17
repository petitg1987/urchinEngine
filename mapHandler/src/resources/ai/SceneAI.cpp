#include "SceneAI.h"
#include <resources/ai/NavMeshAgentReaderWriter.h>

namespace urchin {

    SceneAI::SceneAI(AIEnvironment* aiEnvironment) :
            aiEnvironment(aiEnvironment) {

    }

    const NavMeshAgent* SceneAI::getNavMeshAgent() const {
        return aiEnvironment->getNavMeshGenerator().getNavMeshAgent();
    }

    void SceneAI::changeNavMeshAgent(std::unique_ptr<NavMeshAgent> navMeshAgent) {
        aiEnvironment->getNavMeshGenerator().setNavMeshAgent(std::move(navMeshAgent));
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
