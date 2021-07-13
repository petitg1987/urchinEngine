#pragma once

#include <memory>

#include <UrchinAIEngine.h>

namespace urchin {

    class SceneAI {
        public:
            friend class Map;

            explicit SceneAI(AIManager*);

            const NavMeshAgent* getNavMeshAgent() const;
            void changeNavMeshAgent(std::unique_ptr<NavMeshAgent>);

        private:
            void loadFrom(const DataChunk*, const DataParser&);
            void writeOn(DataChunk&, UdaWriter&) const;

            static constexpr char NAV_MESH_AGENT_TAG[] = "navMeshAgent";

            AIManager* aiManager;
    };

}
