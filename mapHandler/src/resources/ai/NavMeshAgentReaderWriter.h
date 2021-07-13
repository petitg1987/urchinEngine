#pragma once

#include <memory>

#include <UrchinAIEngine.h>

namespace urchin {

    class NavMeshAgentReaderWriter {
        public:
            static std::unique_ptr<NavMeshAgent> loadFrom(const DataChunk*, const DataParser&);
            static void writeOn(DataChunk&, const NavMeshAgent*, UdaWriter&);

        private:
            static constexpr char AGENT_HEIGHT_TAG[] = "agentHeight";
            static constexpr char AGENT_RADIUS_TAG[] = "agentRadius";
            static constexpr char MAX_SLOPE_IN_RADIAN_TAG[] = "maxSlopeInRadian";
            static constexpr char JUMP_DISTANCE_TAG[] = "jumpDistance";
    };

}
