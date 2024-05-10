#pragma once

#include <vector>
#include <optional>
#include <vulkan/vulkan.h>

#include <UrchinCommon.h>
#include <graphics/api/vulkan/texture/Texture.h>
#include <graphics/api/vulkan/render/TextureCopier.h>

namespace urchin {

    class PipelineProcessor;
    class OffscreenRender;

    class RenderTarget {
        public:
            friend class PipelineBuilder;

            enum DepthAttachmentType {
                NO_DEPTH_ATTACHMENT, //no depth attachment
                LOCAL_DEPTH_ATTACHMENT, //depth attachment which cannot be used outside this render target
                SHARED_DEPTH_ATTACHMENT, //depth attachment which can be read and write outside this render target
                EXTERNAL_DEPTH_ATTACHMENT //existing depth attachment from another render target (must be provided via 'setExternalDepthTexture')
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
            std::size_t getRenderPassCompatibilityId() const;

            bool hasDepthAttachment() const;
            DepthAttachmentType getDepthAttachmentType() const;
            void setExternalDepthTexture(const std::shared_ptr<Texture>&);
            const std::shared_ptr<Texture>& getDepthTexture() const;

            void addPreRenderTextureCopier(TextureCopier);
            void removeAllPreRenderTextureCopiers();
            void addProcessor(PipelineProcessor*);
            void removeProcessor(const PipelineProcessor*);
            bool couldHaveGraphicsProcessors() const;
            void notifyProcessorEnabled(const PipelineProcessor*);
            void notifyProcessorDisabled(const PipelineProcessor*);
            void disableAllProcessors() const;

            virtual void render(std::uint32_t, unsigned int) = 0;

        protected:
            struct WaitSemaphore {
                VkSemaphore waitSemaphore;
                VkPipelineStageFlagBits waitDstStageMask;
            };

            void initializeProcessors();
            void cleanupProcessors() const;
            std::span<PipelineProcessor* const> getProcessors() const;
            bool hasProcessor() const;
            bool areProcessorsOrCopiersDirty() const;

            VkAttachmentDescription2 buildDepthAttachment(VkImageLayout) const;
            VkAttachmentDescription2 buildAttachment(VkFormat, bool, bool, VkImageLayout) const;
            void createRenderPass(const VkAttachmentReference2&, const std::vector<VkAttachmentReference2>&, const std::vector<VkAttachmentDescription2>&);
            void destroyRenderPass();
            void createDepthResources();
            void destroyDepthResources();
            void addNewFrameBuffer(std::span<VkImageView>);
            void destroyFramebuffers();
            void createCommandBuffers();
            void createCommandPool();
            void destroyCommandBuffersAndPool();

            VkRenderPass getRenderPass() const;
            std::span<OffscreenRender*> getOffscreenRenderDependencies() const;
            void configureWaitSemaphore(std::uint32_t, VkSubmitInfo&, std::optional<WaitSemaphore>) const;

            virtual bool needCommandBufferRefresh(std::size_t) const = 0;
            virtual void waitCommandBuffersIdle() const = 0;
            void updatePipelineProcessorData(uint32_t) const;
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

            mutable std::vector<OffscreenRender*> offscreenRenderDependencies;
            mutable std::vector<VkSemaphore> queueSubmitWaitSemaphores;
            mutable std::vector<VkPipelineStageFlags> queueSubmitWaitStages;

            std::vector<TextureCopier> preRenderTextureCopier;
            bool copiersDirty;
            std::vector<PipelineProcessor*> processors;
            std::vector<PipelineProcessor*> sortedEnabledProcessors;
            bool processorsDirty;
    };

}
