#ifndef URCHINENGINE_OFFSCREENRENDERER_H
#define URCHINENGINE_OFFSCREENRENDERER_H

#include <memory>

#include "graphic/texture/Texture.h"
#include "graphic/render/GenericRenderer.h"

namespace urchin {

    class OffScreenRenderer {
        public:
            OffScreenRenderer();
            ~OffScreenRenderer();

            void addTexture(const std::shared_ptr<Texture> &);

            void clearTextures() const;
            void draw(const std::unique_ptr<GenericRenderer> &) const;

        private:
            unsigned int framebufferId;

            std::shared_ptr<Texture> depthTexture;
            std::vector<std::shared_ptr<Texture>> textures;
            std::vector<unsigned int> allAttachmentsIndex;
    };

}

#endif
