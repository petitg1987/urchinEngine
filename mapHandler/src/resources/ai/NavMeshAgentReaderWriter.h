#pragma once

#include <memory>

#include <UrchinAIEngine.h>

namespace urchin {

    class NavMeshAgentReaderWriter {
        public:
            static std::shared_ptr<NavMeshAgent> loadFrom(const std::shared_ptr<XmlChunk>&, const XmlParser&);
            static void writeOn(const std::shared_ptr<XmlChunk>&, const std::shared_ptr<const NavMeshAgent>&, XmlWriter&);

        private:
            static constexpr char AGENT_HEIGHT_TAG[] = "agentHeight";
            static constexpr char AGENT_RADIUS_TAG[] = "agentRadius";
            static constexpr char MAX_SLOPE_IN_RADIAN_TAG[] = "maxSlopeInRadian";
            static constexpr char JUMP_DISTANCE_TAG[] = "jumpDistance";
    };

}
