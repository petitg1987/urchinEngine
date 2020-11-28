#include "RenderTarget.h"

namespace urchin {

    RenderTarget::RenderTarget() :
        targetWidth(0),
        targetHeight(0) {

    }

    void RenderTarget::executeRenderer(const std::unique_ptr<GenericRenderer> &renderer) const {
        renderer->render();
    }

    unsigned int RenderTarget::getTargetWidth() const {
        return targetWidth;
    }

    unsigned int RenderTarget::getTargetHeight() const {
        return targetHeight;
    }

    void RenderTarget::onResize(unsigned int targetWidth, unsigned int targetHeight) {
        this->targetWidth = targetWidth;
        this->targetHeight = targetHeight;
    }

    void RenderTarget::activeShader(const std::unique_ptr<Shader> &shader) const {
        shader->bind();
    }

}