#include <stdexcept>

#include <graphic/render/blend/BlendFunction.h>

namespace urchin {

    BlendFunction::BlendFunction(bool isBlendEnabled, BlendFactor srcColorFactor, BlendFactor dstColorFactor, BlendFactor srcAlphaFactor, BlendFactor dstAlphaFactor) :
            bIsBlendEnabled(isBlendEnabled),
            srcColorFactor(srcColorFactor),
            dstColorFactor(dstColorFactor),
            srcAlphaFactor(srcAlphaFactor),
            dstAlphaFactor(dstAlphaFactor) {

    }

    BlendFunction BlendFunction::build(BlendFactor srcColorFactor, BlendFactor dstColorFactor, BlendFactor srcAlphaFactor, BlendFactor dstAlphaFactor) {
        return BlendFunction(true, srcColorFactor, dstColorFactor, srcAlphaFactor, dstAlphaFactor);
    }

    /**
     * Default blend functions:
     *     - target.rgb = src.a * src.rgb + (1 - src.a) * target.rgb
     *     - target.a = 1 * src.a + 0 * target.a
     * where "target" is the target buffer to draw on and "src" is the object/model to draw in the target buffer.
     */
    BlendFunction BlendFunction::buildDefault() {
        return BlendFunction(true, BlendFactor::SRC_ALPHA, BlendFactor::ONE_MINUS_SRC_ALPHA, BlendFactor::ONE, BlendFactor::ZERO);
    }

    BlendFunction BlendFunction::buildBlendDisabled() {
        return BlendFunction(false, BlendFactor::ONE, BlendFactor::ZERO, BlendFactor::ONE, BlendFactor::ZERO);
    }

    BlendFactor BlendFunction::getSrcColorFactor() const {
        return srcColorFactor;
    }

    BlendFactor BlendFunction::getDstColorFactor() const {
        return dstColorFactor;
    }

    BlendFactor BlendFunction::getSrcAlphaFactor() const {
        return srcAlphaFactor;
    }

    BlendFactor BlendFunction::getDstAlphaFactor() const {
        return dstAlphaFactor;
    }

    void BlendFunction::setupColorBlend(VkPipelineColorBlendAttachmentState& colorBlendAttachment) const {
        colorBlendAttachment.blendEnable = bIsBlendEnabled ? VK_TRUE : VK_FALSE;

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
        throw std::runtime_error("Unknown blend factor: " + std::to_string((int)blendFactor));
    }

}
