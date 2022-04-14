#pragma once

#include <vector>
#include <optional>
#include <vulkan/vulkan.h>

#include <UrchinCommon.h>
#include <api/texture/Texture.h>


namespace urchin {

    class GenericRenderer;
    class OffscreenRender;

    class RenderTarget {
        public:
            enum DepthAttachmentType {
                NO_DEPTH_ATTACHMENT, //no depth attachment
                LOCAL_DEPTH_ATTACHMENT, //depth attachment which cannot be used outside this render target
                SHARED_DEPTH_ATTACHMENT, //depth attachment which can be read and write outside this render target
                EXTERNAL_DEPTH_ATTACHMENT //existing depth attachment from another render target (must be provided via 'setExternalDepthTexture')
            };
            struct WaitSemaphore {
                VkSemaphore waitSemaphore;
                VkPipelineStageFlagBits waitDstStageMask;
            };

            explicit RenderTarget(std::string, DepthAttachmentType);
            virtual ~RenderTarget();

            virtual void initialize() = 0;
            virtual void cleanup() = 0;

            std::string getName() const;
            virtual bool isValidRenderTarget() const;
            virtual unsigned int getWidth() const = 0;
            virtual unsigned int getHeight() const = 0;
            virtual unsigned int getLayer() const = 0;
            virtual std::size_t getNumFramebuffer() const = 0;
            virtual std::size_t getNumColorAttachment() const = 0;
            virtual std::size_t hasOutputTextureWithContentToLoad() const = 0;
            VkRenderPass getRenderPass() const;
            std::size_t getRenderPassCompatibilityId() const;

            bool hasDepthAttachment() const;
            DepthAttachmentType getDepthAttachmentType() const;
            void setExternalDepthTexture(const std::shared_ptr<Texture>&);
            const std::shared_ptr<Texture>& getDepthTexture() const;

            void addRenderer(GenericRenderer*);
            void removeRenderer(const GenericRenderer*);
            void notifyRendererEnabled(const GenericRenderer*);
            void notifyRendererDisabled(const GenericRenderer*);
            void disableAllRenderers() const;

            virtual void render(std::uint64_t, unsigned int) = 0;

        protected:
            void initializeRenderers();
            void cleanupRenderers() const;
            std::span<GenericRenderer* const> getRenderers() const;
            bool hasRenderer() const;
            bool areRenderersDirty() const;

            VkAttachmentDescription buildDepthAttachment(VkImageLayout) const;
            VkAttachmentDescription buildAttachment(VkFormat, bool, bool, VkImageLayout) const;
            void createRenderPass(const VkAttachmentReference&, const std::vector<VkAttachmentReference>&, const std::vector<VkAttachmentDescription>&);
            void destroyRenderPass();
            void createDepthResources();
            void destroyDepthResources();
            void addNewFrameBuffer(std::span<VkImageView>);
            void destroyFramebuffers();
            void createCommandBuffers();
            void createCommandPool();
            void destroyCommandBuffersAndPool();

            std::span<OffscreenRender*> getRenderDependencies() const;
            void configureWaitSemaphore(std::uint64_t, VkSubmitInfo&, std::optional<WaitSemaphore>) const;

            virtual bool needCommandBufferRefresh(std::size_t) const = 0;
            virtual void waitCommandBuffersIdle() const = 0;
            void updateGraphicData(uint32_t) const;
            void updateCommandBuffers(uint32_t, const std::vector<VkClearValue>&);

            bool isInitialized;

            std::shared_ptr<Texture> depthTexture;
            std::vector<VkCommandBuffer> commandBuffers;

        private:
            std::string name;
            DepthAttachmentType depthAttachmentType;
            std::shared_ptr<Texture> externalDepthTexture;
            VkRenderPass renderPass;
            std::size_t renderPassCompatibilityId;
            std::vector<VkFramebuffer> framebuffers;
            VkCommandPool commandPool;

            mutable std::vector<OffscreenRender*> renderDependencies;
            mutable std::vector<VkSemaphore> queueSubmitWaitSemaphores;
            mutable std::vector<VkPipelineStageFlags> queueSubmitWaitStages;

            std::vector<GenericRenderer*> renderers;
            bool renderersDirty;
    };

}
