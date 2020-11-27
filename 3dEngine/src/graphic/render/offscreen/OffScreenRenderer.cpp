#include <GL/glew.h>

#include "OffScreenRenderer.h"

namespace urchin {

    OffScreenRenderer::OffScreenRenderer() :
            framebufferId(0) {
        glGenFramebuffers(1, &framebufferId);

        glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);
        glReadBuffer(GL_NONE);
    }

    OffScreenRenderer::~OffScreenRenderer() {
        glDeleteFramebuffers(1, &framebufferId);
    }

    void OffScreenRenderer::addTexture(const std::shared_ptr<Texture> &texture) {
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);

        if(texture->getTextureFormat() == TextureFormat::DEPTH_16_FLOAT
                || texture->getTextureFormat() == TextureFormat::DEPTH_24_FLOAT
                || texture->getTextureFormat() == TextureFormat::DEPTH_32_FLOAT) {
            if(depthTexture) {
                throw std::runtime_error("Offscreen renderer doesn't support several depth textures attachment.");
            }

            depthTexture = texture;

            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture->getTextureId(), 0);
        } else {
            textures.push_back(texture);

            unsigned int attachmentIndex = GL_COLOR_ATTACHMENT0 + textures.size() - 1;
            glFramebufferTexture(GL_FRAMEBUFFER, attachmentIndex, texture->getTextureId(), 0);
        }
    }

    void OffScreenRenderer::apply(const std::unique_ptr<GenericRenderer> &renderer) {
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);
        //glDrawBuffers(1, &fboAttachments[2]);

        renderer->draw();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

}
