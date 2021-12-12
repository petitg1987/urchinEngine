#pragma once

#include <memory>

#include <UrchinAIEngine.h>

namespace urchin {

    class NavMeshAgentReaderWriter { //TODO class in wrong folder
        public:
            static std::unique_ptr<NavMeshAgent> loadNavMeshAgent(const UdaChunk*, const UdaParser&);
            static void writeNavMeshAgent(UdaChunk&, const NavMeshAgent*, UdaWriter&);

        private:
            static constexpr char NAV_MESH_AGENT_TAG[] = "navMeshAgent";
            static constexpr char AGENT_HEIGHT_TAG[] = "agentHeight";
            static constexpr char AGENT_RADIUS_TAG[] = "agentRadius";
            static constexpr char MAX_SLOPE_IN_RADIAN_TAG[] = "maxSlopeInRadian";
            static constexpr char JUMP_DISTANCE_TAG[] = "jumpDistance";
    };

}
