#include "TargetRenderer.h"

namespace urchin {

    TargetRenderer::TargetRenderer() :
        targetWidth(0),
        targetHeight(0) {

    }

    unsigned int TargetRenderer::getTargetWidth() const {
        return targetWidth;
    }

    unsigned int TargetRenderer::getTargetHeight() const {
        return targetHeight;
    }

    void TargetRenderer::onResize(unsigned int targetWidth, unsigned int targetHeight) {
        this->targetWidth = targetWidth;
        this->targetHeight = targetHeight;
    }

}