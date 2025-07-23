#include "graphics/render/PipelineProcessorComparator.h"
#include "graphics/api/GraphicsApi.h"

namespace urchin {

    bool PipelineProcessorComparator::operator()(const PipelineProcessor* lhs, const PipelineProcessor* rhs) const {
        if (lhs->getRenderingOrder() == rhs->getRenderingOrder()) {
            if (lhs->isDepthTestEnabled() == rhs->isDepthTestEnabled()) {
                if (lhs->getPipelineId() == rhs->getPipelineId()) {
                    //only to ensure same ordering between frames
                    return lhs < rhs;
                }
                return lhs->getPipelineId() < rhs->getPipelineId();
            }
            //always render last the entities with depth test disabled to have a consistent rendering
            return lhs->isDepthTestEnabled() && !rhs->isDepthTestEnabled();
        }
        return lhs->getRenderingOrder() < rhs->getRenderingOrder();
    }

}