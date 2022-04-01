#pragma once

#include <UrchinCommon.h>
#include <UrchinAggregation.h>

#include <controller/AbstractController.h>

namespace urchin {

    class AIController : public AbstractController {
        public:
            AIController();

            const NavMeshAgentEntity& getNavMeshAgentEntity() const;
            void updateNavMeshAgentEntity(const NavMeshAgent&);
    };

}
