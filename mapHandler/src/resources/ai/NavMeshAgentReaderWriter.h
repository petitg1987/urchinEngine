#pragma once

#include <memory>

#include <UrchinAIEngine.h>

namespace urchin {

    class NavMeshAgentReaderWriter {
        //XML tags
        #define AGENT_HEIGHT_TAG "agentHeight"
        #define AGENT_RADIUS_TAG "agentRadius"
        #define MAX_SLOPE_IN_RADIAN_TAG "maxSlopeInRadian"
        #define JUMP_DISTANCE_TAG "jumpDistance"

        public:
            static std::shared_ptr<NavMeshAgent> loadFrom(const std::shared_ptr<XmlChunk>&, const XmlParser&);
            static void writeOn(const std::shared_ptr<XmlChunk>&, const std::shared_ptr<const NavMeshAgent>&, XmlWriter&);
    };

}
