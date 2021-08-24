#include <scene/renderer3d/postprocess/bloom/BloomEffectApplier.h>
#include <graphic/render/target/OffscreenRender.h>

namespace urchin {

    void BloomEffectApplier::onTextureUpdate(const std::shared_ptr<Texture>& lightingPassTexture, std::optional<RenderTarget*> customRenderTarget) {
        this->sceneWidth = lightingPassTexture->getWidth(); //TODO init in cons
        this->sceneHeight = lightingPassTexture->getHeight();

        if (customRenderTarget.has_value()) {
            this->finalRenderTarget = customRenderTarget.value();
        } else {
            bloomedTexture = Texture::build(sceneWidth, sceneHeight, TextureFormat::RGBA_8_INT, nullptr);
            auto offscreenBloomRenderTarget = std::make_unique<OffscreenRender>("bloom - combine", RenderTarget::NO_DEPTH_ATTACHMENT); //TODO move in constructor
            offscreenBloomRenderTarget->resetTextures();
            offscreenBloomRenderTarget->addTexture(lightingPassTexture);
            offscreenBloomRenderTarget->initialize();

            this->finalRenderTarget = offscreenBloomRenderTarget.get();
        }
    }

    const std::shared_ptr<Texture>& BloomEffectApplier::getBloomedTexture() const {
        if (!bloomedTexture) {
            throw std::runtime_error("No bloomed texture available because rendering has been done in a custom render target");
        }

        return bloomedTexture;
    }

    void BloomEffectApplier::applyBloom() {
        //TODO impl
    }

}