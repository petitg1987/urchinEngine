#pragma once

#include <memory>

#include <graphic/texture/Texture.h>
#include <graphic/render/target/RenderTarget.h>

namespace urchin {

    class BloomEffectApplier {
        public:
            void onTextureUpdate(const std::shared_ptr<Texture>&, std::optional<RenderTarget*>);
            const std::shared_ptr<Texture>& getBloomedTexture() const;

            void applyBloom();

        private:
            unsigned int sceneWidth, sceneHeight;

            std::optional<RenderTarget*> finalRenderTarget;
            std::shared_ptr<Texture> bloomedTexture;
    };

}