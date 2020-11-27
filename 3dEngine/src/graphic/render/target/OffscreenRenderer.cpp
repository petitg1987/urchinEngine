#include <GL/glew.h>

#include "OffscreenRenderer.h"

namespace urchin {

    OffscreenRenderer::OffscreenRenderer() :
            framebufferId(0) {
        glGenFramebuffers(1, &framebufferId);

        glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    OffscreenRenderer::~OffscreenRenderer() {
        glDeleteFramebuffers(1, &framebufferId);
    }

    void OffscreenRenderer::addTexture(const std::shared_ptr<Texture> &texture) {
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

            attachmentsIndices.emplace_back(attachmentIndex);
            glDrawBuffers(attachmentsIndices.size(), &attachmentsIndices[0]);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void OffscreenRenderer::removeAllTextures() {
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 0, 0);
        for (unsigned int i = 0; i<attachmentsIndices.size(); ++i) {
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, 0, 0);
        }
        glDrawBuffers(0, &attachmentsIndices[0]);

        depthTexture = nullptr;

        textures.clear();
        attachmentsIndices.clear();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void OffscreenRenderer::resetDraw() const {
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);

        glClear((unsigned int)GL_DEPTH_BUFFER_BIT | (unsigned int)GL_COLOR_BUFFER_BIT);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void OffscreenRenderer::draw(const std::unique_ptr<GenericRenderer> &renderer) const {
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);
        glViewport(0, 0, getTargetWidth(), getTargetHeight());

        renderer->draw();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

}
