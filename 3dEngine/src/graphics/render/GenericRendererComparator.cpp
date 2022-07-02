#include <graphics/render/GenericRendererComparator.h>
#include <graphics/api/vulkan/render/GenericRenderer.h>

namespace urchin {

    bool GenericRendererComparator::operator()(const GenericRenderer* lhs, const GenericRenderer* rhs) const {
        if (lhs->getRenderingOrder() == rhs->getRenderingOrder()) {
            if (lhs->isDepthTestEnabled() == rhs->isDepthTestEnabled()) {
                if (lhs->getPipelineId() == rhs->getPipelineId()) {
                    //only to ensure same ordering between frames
                    return lhs < rhs;
                }
                return lhs->getPipelineId() < rhs->getPipelineId();
            }
            //always render last the entities with depth test disabled to have a consistent rendering
            if (lhs->isDepthTestEnabled() && !rhs->isDepthTestEnabled()) {
                return true;
            } else {
                return false;
            }
        }
        return lhs->getRenderingOrder() < rhs->getRenderingOrder();
    }

}