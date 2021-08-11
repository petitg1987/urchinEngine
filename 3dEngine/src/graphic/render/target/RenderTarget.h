#pragma once

#include <vector>
#include <forward_list>
#include <vulkan/vulkan.h>

#include <UrchinCommon.h>
#include <graphic/texture/Texture.h>

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
            VkRenderPass getRenderPass() const;

            bool hasDepthAttachment() const;
            void setExternalDepthTexture(const std::shared_ptr<Texture>&);
            const std::shared_ptr<Texture>& getDepthTexture() const;

            void addRenderer(GenericRenderer*);
            void removeRenderer(const GenericRenderer*);
            void notifyRendererEnabled(GenericRenderer*);
            void notifyRendererDisabled(GenericRenderer*);
            void disableAllRenderers();

            virtual void render() = 0;

        protected:
            void initializeRenderers();
            void cleanupRenderers();
            bool hasRenderer();

            VkAttachmentDescription buildDepthAttachment(VkImageLayout) const;
            static VkAttachmentDescription buildAttachment(VkFormat, bool, VkImageLayout);
            void createRenderPass(const VkAttachmentReference&, const std::vector<VkAttachmentReference>&, const std::vector<VkAttachmentDescription>&);
            void destroyRenderPass();
            void createDepthResources();
            void destroyDepthResources();
            void addNewFrameBuffer(const std::vector<VkImageView>&);
            void destroyFramebuffers();
            void createCommandBuffers();
            void createCommandPool();
            void destroyCommandBuffersAndPool();

            const std::vector<OffscreenRender*>& getRenderDependencies() const;
            void configureWaitSemaphore(VkSubmitInfo&, VkSemaphore) const;

            virtual void waitCommandBuffersIdle() const = 0;
            void updateGraphicData(uint32_t);
            void updateCommandBuffers(const std::vector<VkClearValue>&);

            bool isInitialized;

            std::shared_ptr<Texture> depthTexture;
            std::vector<VkCommandBuffer> commandBuffers;

        private:
            bool needCommandBuffersRefresh();

            std::string name;
            DepthAttachmentType depthAttachmentType;
            std::shared_ptr<Texture> externalDepthTexture;
            VkRenderPass renderPass;
            std::vector<VkFramebuffer> framebuffers;
            VkCommandPool commandPool;

            mutable std::vector<OffscreenRender*> renderDependencies;
            mutable std::vector<VkSemaphore> queueSubmitWaitSemaphores;
            mutable std::vector<VkPipelineStageFlags> queueSubmitWaitStages;

            std::list<GenericRenderer*> renderers;
            bool renderersDirty;
    };

}
