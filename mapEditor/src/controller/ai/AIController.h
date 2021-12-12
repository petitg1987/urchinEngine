#pragma once

#include <UrchinCommon.h>
#include <UrchinAggregation.h>

#include <controller/AbstractController.h>

namespace urchin {

    class AIController : public AbstractController {
        public:
            AIController();

            const NavMeshAgent* getNavMeshAgent() const;
            void updateNavMeshAgent(std::unique_ptr<NavMeshAgent>);
    };

}
