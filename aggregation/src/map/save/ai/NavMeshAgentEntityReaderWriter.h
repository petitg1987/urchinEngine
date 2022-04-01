#pragma once

#include <memory>

#include <model/NavMeshAgentEntity.h>

namespace urchin {

    class NavMeshAgentEntityReaderWriter {
        public:
            static std::unique_ptr<NavMeshAgentEntity> load(const UdaChunk*, const UdaParser&);
            static void write(UdaChunk&, const NavMeshAgentEntity&, UdaWriter&);

        private:
            static constexpr char NAV_MESH_AGENT_TAG[] = "navMeshAgent";
            static constexpr char AGENT_HEIGHT_TAG[] = "agentHeight";
            static constexpr char AGENT_RADIUS_TAG[] = "agentRadius";
            static constexpr char MAX_SLOPE_IN_RADIAN_TAG[] = "maxSlopeInRadian";
            static constexpr char JUMP_DISTANCE_TAG[] = "jumpDistance";
    };

}
