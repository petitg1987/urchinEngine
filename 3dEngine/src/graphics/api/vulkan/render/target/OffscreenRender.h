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
        bool enabled;
        std::shared_ptr<Texture> texture;
        LoadType loadOperation;
        std::optional<Vector4<float>> clearColor;
        OutputUsage outputUsage;
    };

    class OffscreenRender final : public RenderTarget {
        public:
            friend class RenderTarget;
            friend class ScreenRender;

            explicit OffscreenRender(std::string, bool, DepthAttachmentType);
            ~OffscreenRender() override;

            void setOutputSize(unsigned int, unsigned int, unsigned int, bool);
            void addOutputTexture(const std::shared_ptr<Texture>&, LoadType = LoadType::NO_LOAD, const std::optional<Vector4<float>>& = std::nullopt, OutputUsage = OutputUsage::GRAPHICS);
            std::shared_ptr<Texture>& getOutputTexture(std::size_t);
            void enableOnlyOutputTexture(const std::shared_ptr<Texture>&);
            void resetOutput();

            void initialize() override;

            unsigned int getWidth() const override;
            unsigned int getHeight() const override;
            unsigned int getLayer() const override;
            bool isArrayOutput() const override;
            std::size_t getNumFramebuffer() const override;
            std::size_t getNumColorAttachment() const override;

            void render(uint32_t, unsigned int) override;

        private:
            void cleanup() override;

            void initializeClearValues();
            void createRenderPass();
            void createFramebuffers();
            void createFence();
            void destroyFence() const;
            void createSemaphores();
            void destroySemaphores() const;

            void updateTexturesWriter();

            void fillAdditionalOffscreenRenderDependencies(std::vector<OffscreenRender*> &) const override;
            VkSemaphore popSubmitSemaphore(uint32_t, const std::string&);
            void markSubmitSemaphoreUnused(uint32_t);

            bool needCommandBufferRefresh(std::size_t) const override;
            void waitCommandBuffersIdle() const override;

            static constexpr std::size_t MAX_SUBMIT_SEMAPHORES = 10;

            unsigned int width;
            unsigned int height;
            unsigned int layer;
            bool bIsArrayOutput;

            std::vector<VkClearValue> clearValues;
            std::vector<OutputTexture> outputTextures;

            VkFence commandBufferFence;
            std::array<VkSemaphore, MAX_SUBMIT_SEMAPHORES> submitSemaphores;
            uint32_t submitSemaphoresFrameCount;
            unsigned int remainingSubmitSemaphores;
            bool submitSemaphoresStale;
    };

}
