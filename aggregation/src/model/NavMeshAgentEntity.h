#pragma once

#include <UrchinAIEngine.h>

namespace urchin {

    class NavMeshAgentEntity {
        public:
            friend class Map;

            NavMeshAgentEntity();

            const NavMeshAgent& getNavMeshAgent() const;
            void setNavMeshAgent(std::unique_ptr<NavMeshAgent>);

        private:
            void setup(AIEnvironment*);

            AIEnvironment* aiEnvironment;

            std::unique_ptr<NavMeshAgent> navMeshAgent;
    };

}
