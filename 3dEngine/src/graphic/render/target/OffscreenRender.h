#pragma once

#include <vulkan/vulkan.h>

#include <graphic/render/target/RenderTarget.h>
#include <graphic/texture/Texture.h>

namespace urchin {

    enum LoadType {
        NO_LOAD, //texture content is not loaded (default)
        LOAD_CLEAR, //texture content is cleared at pipeline start
        LOAD_CONTENT //texture content is loaded at pipeline start. Use case: blending must be applied on an existing texture having content
    };

    struct OutputTexture {
        std::shared_ptr<Texture> texture;
        LoadType loadOperation;
        std::optional<Vector4<float>> clearColor;
    };

    class OffscreenRender : public RenderTarget {
        public:
            explicit OffscreenRender(std::string, DepthAttachmentType);
            ~OffscreenRender() override;

            void addOutputTexture(const std::shared_ptr<Texture>&, LoadType = LoadType::NO_LOAD, std::optional<Vector4<float>> = std::nullopt);
            void resetOutputTextures();

            void initialize() override;
            void cleanup() override;

            unsigned int getWidth() const override;
            unsigned int getHeight() const override;
            unsigned int getLayer() const override;
            std::size_t getNumFramebuffer() const override;
            std::size_t getNumColorAttachment() const override;

            VkSemaphore retrieveQueueSubmitSemaphoreAndFlagUsed();

            void render() override;

        private:
            void initializeClearValues();
            void createRenderPass();
            void createFramebuffers();
            void createSyncObjects();
            void destroySyncObjects();

            void updateTexturesWriter();
            void waitCommandBuffersIdle() const override;

            std::vector<VkClearValue> clearValues;
            std::vector<OutputTexture> outputTextures;

            VkSemaphore queueSubmitSemaphore;
            bool queueSubmitSemaphoreUsable;
            VkFence commandBufferFence;
    };

}
