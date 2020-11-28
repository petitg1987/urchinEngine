#ifndef URCHINENGINE_OFFSCREENRENDERER_H
#define URCHINENGINE_OFFSCREENRENDERER_H

#include <memory>

#include "graphic/render/target/RenderTarget.h"
#include "graphic/texture/Texture.h"
#include "graphic/render/GenericRenderer.h"

namespace urchin {

    class OffscreenRender : public RenderTarget {
        public:
            OffscreenRender();
            ~OffscreenRender() override;

            void addTexture(const std::shared_ptr<Texture> &);
            void removeAllTextures();

            void resetDraw() const override;
            void display(const std::unique_ptr<GenericRenderer> &) const override;

        private:
            unsigned int framebufferId;

            std::shared_ptr<Texture> depthTexture;
            std::vector<std::shared_ptr<Texture>> textures;
            std::vector<unsigned int> attachmentsIndices;
    };

}

#endif
