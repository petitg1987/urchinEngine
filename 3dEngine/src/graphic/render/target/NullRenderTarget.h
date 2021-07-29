#pragma once

#include <graphic/render/target/RenderTarget.h>

namespace urchin {

    /**
     * Render target which does nothing (useful for testing)
     */
    class NullRenderTarget : public RenderTarget {
        public:
            NullRenderTarget(unsigned int, unsigned int);

            void initialize() override;
            void cleanup() override;

            bool isValidRenderTarget() const override;
            unsigned int getWidth() const override;
            unsigned int getHeight() const override;
            unsigned int getLayer() const override;
            std::size_t getNumFramebuffer() const override;
            std::size_t getNumColorAttachment() const override;

            void render();

        private:
            void waitCommandBuffersIdle() const override;

            unsigned int width;
            unsigned int height;
    };

}
