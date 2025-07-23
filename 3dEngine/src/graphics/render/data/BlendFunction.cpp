#include "graphics/render/data/BlendFunction.h"

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
     *     - target.rgb = [src.a] * src.rgb + [1 - src.a] * target.rgb
     *     - target.a = [1] * src.a + [0] * target.a
     * where "target" is the target buffer to draw on and "src" is the object/model to draw in the target buffer.
     */
    BlendFunction BlendFunction::buildDefault() {
        return BlendFunction(true, BlendFactor::SRC_ALPHA, BlendFactor::ONE_MINUS_SRC_ALPHA, BlendFactor::ONE, BlendFactor::ZERO);
    }

    BlendFunction BlendFunction::buildBlendDisabled() {
        return BlendFunction(false, BlendFactor::ONE, BlendFactor::ZERO, BlendFactor::ONE, BlendFactor::ZERO);
    }

    bool BlendFunction::isBlendEnabled() const {
        return bIsBlendEnabled;
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

}
