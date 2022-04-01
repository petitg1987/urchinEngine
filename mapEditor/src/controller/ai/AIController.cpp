#include <memory>
#include <controller/ai/AIController.h>

namespace urchin {

    AIController::AIController() :
            AbstractController() {

    }

    const NavMeshAgent& AIController::getNavMeshAgent() const {
        return getMap().getAIEnvironment()->getNavMeshGenerator().getNavMeshAgent();
    }

    void AIController::updateNavMeshAgent(const NavMeshAgent& navMeshAgent) {
        getMap().getAIEnvironment()->getNavMeshGenerator().setNavMeshAgent(std::move(navMeshAgent));
        markModified();
    }

}
