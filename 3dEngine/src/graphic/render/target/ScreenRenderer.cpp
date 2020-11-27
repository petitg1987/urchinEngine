#include <GL/glew.h>

#include "ScreenRenderer.h"

namespace urchin {

    void ScreenRenderer::resetDraw() const {
        glClear((unsigned int)GL_DEPTH_BUFFER_BIT | (unsigned int)GL_COLOR_BUFFER_BIT);
    }

    void ScreenRenderer::draw(const std::unique_ptr<GenericRenderer> &renderer) const {
        glViewport(0, 0, getTargetWidth(), getTargetHeight());

        renderer->draw();
    }

}
