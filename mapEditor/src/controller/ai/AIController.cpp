#include <controller/ai/AIController.h>

namespace urchin {

    const NavMeshAgentEntity& AIController::getNavMeshAgentEntity() const {
        return getMap().getNavMeshAgentEntity();
    }

    void AIController::updateNavMeshAgentEntity(const NavMeshAgent& navMeshAgent) {
        auto navMeshAgentEntity = std::make_unique<NavMeshAgentEntity>();
        navMeshAgentEntity->setNavMeshAgent(std::make_unique<NavMeshAgent>(navMeshAgent));
        getMap().setNavMeshAgentEntity(std::move(navMeshAgentEntity));

        markModified();
    }

}
