#include <memory>
#include <controller/ai/AIController.h>

namespace urchin {

    AIController::AIController() :
            AbstractController() {

    }

    const NavMeshAgent* AIController::getNavMeshAgent() const {
        return getMapHandler()->getMap().getAIEnvironment()->getNavMeshGenerator().getNavMeshAgent();
    }

    void AIController::updateNavMeshAgent(std::unique_ptr<NavMeshAgent> navMeshAgent) {
        getMapHandler()->getMap().getAIEnvironment()->getNavMeshGenerator().setNavMeshAgent(std::move(navMeshAgent));
        markModified();
    }

}
