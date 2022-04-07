#include <api/render/target/NullRenderTarget.h>

namespace urchin {

    NullRenderTarget::NullRenderTarget(unsigned int width, unsigned int height) :
            RenderTarget("_null_", RenderTarget::SHARED_DEPTH_ATTACHMENT),
            width(width),
            height(height) {
        NullRenderTarget::initialize();
    }

    void NullRenderTarget::initialize() {
        depthTexture = Texture::build("null render target - depth", getWidth(), getHeight(), TextureFormat::DEPTH_32_FLOAT, nullptr);
        depthTexture->enableTextureWriting();
    }

    void NullRenderTarget::cleanup() {
        destroyDepthResources();
    }

    bool NullRenderTarget::isValidRenderTarget() const {
        return false;
    }

    unsigned int NullRenderTarget::getWidth() const {
        return width;
    }

    unsigned int NullRenderTarget::getHeight() const {
        return height;
    }

    unsigned int NullRenderTarget::getLayer() const {
        return 1;
    }

    std::size_t NullRenderTarget::getNumFramebuffer() const {
        return 0;
    }

    std::size_t NullRenderTarget::getNumColorAttachment() const {
        return 0;
    }

    std::size_t NullRenderTarget::hasOutputTextureWithContentToLoad() const {
        return false;
    }

    bool NullRenderTarget::needCommandBufferRefresh(std::size_t) const {
        return true;
    }

    void NullRenderTarget::waitCommandBuffersIdle() const {
        //nothing to do
    }

    void NullRenderTarget::render() {
        //nothing to do
    }
}
