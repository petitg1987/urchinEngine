#pragma once

#include <memory>

#include <graphic/texture/Texture.h>
#include <graphic/render/target/RenderTarget.h>
#include <graphic/render/shader/model/Shader.h>

namespace urchin {

    class BloomEffectApplier {
        public:
            BloomEffectApplier();
            ~BloomEffectApplier();

            void onTextureUpdate(const std::shared_ptr<Texture>&, std::optional<RenderTarget*>);
            const std::shared_ptr<Texture>& getBloomedTexture() const;

            void applyBloom();

        private:
            void clearRendering();
            void refreshRenderer();

            unsigned int sceneWidth, sceneHeight;
            std::shared_ptr<Texture> lightingPassTexture;

            RenderTarget* finalRenderTarget;
            std::shared_ptr<Texture> bloomedTexture;
            std::unique_ptr<OffscreenRender> offscreenBloomRenderTarget;

            std::unique_ptr<Shader> bloomPreFilterShader;
            std::unique_ptr<GenericRenderer> preFilterRenderer;
    };

}