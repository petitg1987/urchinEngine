#pragma once

#include <UrchinAggregation.h>

#include "controller/AbstractController.h"

namespace urchin {

    class AIController final : public AbstractController {
        public:
            const NavMeshAgentEntity& getNavMeshAgentEntity() const;
            void updateNavMeshAgentEntity(const NavMeshAgent&);
    };

}
