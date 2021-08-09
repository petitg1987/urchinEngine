#include <stdexcept>

#include <graphic/render/blend/BlendFunction.h>

namespace urchin {

    BlendFunction::BlendFunction(BlendFactor srcColorFactor, BlendFactor dstColorFactor, BlendFactor srcAlphaFactor, BlendFactor dstAlphaFactor) :
            srcColorFactor(srcColorFactor),
            dstColorFactor(dstColorFactor),
            srcAlphaFactor(srcAlphaFactor),
            dstAlphaFactor(dstAlphaFactor) {

    }

    BlendFunction BlendFunction::build(BlendFactor srcColorFactor, BlendFactor dstColorFactor, BlendFactor srcAlphaFactor, BlendFactor dstAlphaFactor) {
        return BlendFunction(srcColorFactor, dstColorFactor, srcAlphaFactor, dstAlphaFactor);
    }

    /**
     * Default blend function:
     *   - Color: target.rgb = src.a * src.rgb + (1 - src.a) * target.rgb
     *   - Alpha: target.a = 1 * src.a + 0 * target.a
     * where "target" is the target buffer to draw on and "src" is the object/model to draw in the target buffer.
     */
    BlendFunction BlendFunction::buildDefault() {
        return BlendFunction(SRC_ALPHA, ONE_MINUS_SRC_ALPHA, ONE, ZERO);
    }

    void BlendFunction::setupColorBlendAttachment(VkPipelineColorBlendAttachmentState &colorBlendAttachment) const {
        colorBlendAttachment.srcColorBlendFactor = toVkBlenderFactor(srcColorFactor);
        colorBlendAttachment.dstColorBlendFactor = toVkBlenderFactor(dstColorFactor);
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = toVkBlenderFactor(srcAlphaFactor);
        colorBlendAttachment.dstAlphaBlendFactor = toVkBlenderFactor(dstAlphaFactor);
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
    }

    VkBlendFactor BlendFunction::toVkBlenderFactor(BlendFactor blendFactor) const {
        if (blendFactor == BlendFactor::SRC_ALPHA) {
            return VK_BLEND_FACTOR_SRC_ALPHA;
        } else if (blendFactor == BlendFactor::ONE_MINUS_SRC_ALPHA) {
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        } else if (blendFactor == BlendFactor::ONE_MINUS_SRC_COLOR) {
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        } else if (blendFactor == BlendFactor::ONE) {
            return VK_BLEND_FACTOR_ONE;
        } else if (blendFactor == BlendFactor::ZERO) {
            return VK_BLEND_FACTOR_ZERO;
        }
        throw std::runtime_error("Unknown blend factor: " + std::to_string(blendFactor));
    }

}
