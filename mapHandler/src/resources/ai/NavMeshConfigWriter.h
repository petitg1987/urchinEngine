#ifndef URCHINENGINE_NAVMESHCONFIGWRITER_H
#define URCHINENGINE_NAVMESHCONFIGWRITER_H

#include <memory>

#include "UrchinAIEngine.h"

namespace urchin
{

    class NavMeshConfigWriter
    {
        //XML tags
        #define MAX_SLOPE_IN_RADIAN_TAG "maxSlopeInRadian"
        #define AGENT_TAG "agent"
        #define AGENT_HEIGHT_TAG "agentHeight"
        #define AGENT_RADIUS_TAG "agentRadius"

        public:
            std::shared_ptr<NavMeshConfig> loadFrom(const std::shared_ptr<XmlChunk> &, const XmlParser &) const;
            void writeOn(const std::shared_ptr<XmlChunk> &, const std::shared_ptr<const NavMeshConfig> &, XmlWriter &) const;
    };

}

#endif
