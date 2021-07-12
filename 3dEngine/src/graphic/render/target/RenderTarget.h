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
                WRITE_ONLY_DEPTH_ATTACHMENT, //depth attachment not readable outside the render target
                READ_WRITE_DEPTH_ATTACHMENT //depth attachment readable outside the render target
            };

            explicit RenderTarget(std::string, DepthAttachmentType);
            virtual ~RenderTarget();

            virtual void initialize() = 0;
            virtual void cleanup() = 0;

            std::string getName() const;
            virtual unsigned int getWidth() const = 0;
            virtual unsigned int getHeight() const = 0;
            virtual unsigned int getLayer() const = 0;
            virtual std::size_t getNumFramebuffer() const = 0;
            virtual std::size_t getNumColorAttachment() const = 0;
            VkRenderPass getRenderPass() const;

            bool hasDepthAttachment() const;
            bool isReadableDepthAttachment() const;
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

            std::shared_ptr<Texture> depthTexture;
            std::vector<VkCommandBuffer> commandBuffers;

        private:
            bool needCommandBuffersRefresh();

            std::string name;
            DepthAttachmentType depthAttachmentType;
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
