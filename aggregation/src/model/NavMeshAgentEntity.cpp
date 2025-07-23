#include "model/NavMeshAgentEntity.h"

namespace urchin {

    NavMeshAgentEntity::NavMeshAgentEntity() :
            aiEnvironment(nullptr) {
        //default nav mesh agent
        navMeshAgent = std::make_unique<NavMeshAgent>();
    }

    void NavMeshAgentEntity::setup(AIEnvironment* aiEnvironment) {
        this->aiEnvironment = aiEnvironment;

        if (aiEnvironment) {
            aiEnvironment->getNavMeshGenerator().setNavMeshAgent(*navMeshAgent);
        }
    }

    const NavMeshAgent& NavMeshAgentEntity::getNavMeshAgent() const {
        return *navMeshAgent;
    }

    void NavMeshAgentEntity::setNavMeshAgent(std::unique_ptr<NavMeshAgent> navMeshAgent) {
        this->navMeshAgent = std::move(navMeshAgent);

        if (aiEnvironment) {
            aiEnvironment->getNavMeshGenerator().setNavMeshAgent(*this->navMeshAgent);
        }
    }

}
