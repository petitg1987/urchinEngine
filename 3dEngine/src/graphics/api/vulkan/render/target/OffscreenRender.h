#pragma once

#include <vulkan/vulkan.h>
#include <optional>

#include <graphics/api/vulkan/render/target/RenderTarget.h>
#include <graphics/api/vulkan/texture/Texture.h>

namespace urchin {

    enum class LoadType {
        NO_LOAD, //texture content is not loaded (default)
        LOAD_CLEAR, //texture content is cleared at pipeline start
        LOAD_CONTENT //texture content is loaded at pipeline start. Use case: blending must be applied on an existing texture having content
    };

    struct OutputTexture {
        std::shared_ptr<Texture> texture;
        LoadType loadOperation;
        std::optional<Vector4<float>> clearColor;
        OutputUsage outputUsage;
    };

    class OffscreenRender final : public RenderTarget {
        public:
            friend class RenderTarget;
            friend class ScreenRender;

            explicit OffscreenRender(std::string, DepthAttachmentType);
            ~OffscreenRender() override;

            void addOutputTexture(const std::shared_ptr<Texture>&, LoadType = LoadType::NO_LOAD, std::optional<Vector4<float>> = std::nullopt, OutputUsage = OutputUsage::GRAPHICS);
            void resetOutputTextures();
            std::shared_ptr<Texture>& getOutputTexture(std::size_t);

            void initialize() override;
            void cleanup() override;

            unsigned int getWidth() const override;
            unsigned int getHeight() const override;
            unsigned int getLayer() const override;
            std::size_t getNumFramebuffer() const override;
            std::size_t getNumColorAttachment() const override;

            void render(std::uint32_t, unsigned int) override;

        private:
            void initializeClearValues();
            void createRenderPass();
            void createFramebuffers();
            void createFence();
            void destroyFence();
            void createSemaphores();
            void destroySemaphores() const;

            void updateTexturesWriter();

            VkSemaphore popSubmitSemaphore(std::uint32_t);
            void markSubmitSemaphoreUnused(std::uint32_t);

            bool needCommandBufferRefresh(std::size_t) const override;
            void waitCommandBuffersIdle() const override;

            static constexpr std::size_t MAX_SUBMIT_SEMAPHORES = 6;

            std::vector<VkClearValue> clearValues;
            std::vector<OutputTexture> outputTextures;

            VkFence commandBufferFence;
            std::array<VkSemaphore, MAX_SUBMIT_SEMAPHORES> submitSemaphores;
            std::uint32_t submitSemaphoresFrameIndex;
            unsigned int remainingSubmitSemaphores;
            bool submitSemaphoresStale;
    };

}
