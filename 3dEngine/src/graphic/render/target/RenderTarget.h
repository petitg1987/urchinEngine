#ifndef URCHINENGINE_RENDERTARGET_H
#define URCHINENGINE_RENDERTARGET_H

#include <vector>
#include <vulkan/vulkan.h>

#include "UrchinCommon.h"
#include "graphic/texture/Texture.h"

namespace urchin {

    class GenericRenderer;

    class RenderTarget {
        public:
            enum DepthAttachmentType {
                NO_DEPTH_ATTACHMENT, //no depth attachment
                WRITE_ONLY_DEPTH_ATTACHMENT, //depth attachment not readable outside the render target
                READ_WRITE_DEPTH_ATTACHMENT //depth attachment readable outside the render target
            };

            explicit RenderTarget(DepthAttachmentType);
            virtual ~RenderTarget() = default;

            virtual void initialize() = 0;
            virtual void cleanup() = 0;

            virtual unsigned int getWidth() const = 0;
            virtual unsigned int getHeight() const = 0;
            virtual std::size_t getNumFramebuffer() const = 0;
            virtual std::size_t getNumColorAttachment() const = 0;
            VkRenderPass getRenderPass() const;

            bool hasDepthAttachment() const;
            bool isReadableDepthAttachment() const;
            const std::shared_ptr<Texture>& getDepthTexture() const;

            void addRenderer(GenericRenderer*);
            void removeRenderer(GenericRenderer*);
            void clearRenderers();

            virtual void render() = 0;

        protected:
            void initializeRenderers();
            void cleanupRenderers();
            bool hasRenderer();

            VkAttachmentDescription buildDepthAttachment(VkImageLayout) const;
            static VkAttachmentDescription buildAttachment(VkFormat, VkImageLayout);
            void createRenderPass(const VkAttachmentReference&, const std::vector<VkAttachmentReference>&, const std::vector<VkAttachmentDescription>&);
            void destroyRenderPass();
            void createDepthResources();
            void destroyDepthResources();
            void addNewFrameBuffer(const std::vector<VkImageView>&);
            void destroyFramebuffers();
            void createCommandBuffers();
            void createCommandPool();
            void destroyCommandBuffersAndPool();

            virtual void waitCommandBuffersIdle() const = 0;
            void updateGraphicData(uint32_t);
            void updateCommandBuffers(const std::vector<VkClearValue>&);

            std::shared_ptr<Texture> depthTexture;
            std::vector<VkCommandBuffer> commandBuffers;

        private:
            bool needCommandBuffersRefresh() const;

            DepthAttachmentType depthAttachmentType;
            VkRenderPass renderPass;
            std::vector<VkFramebuffer> framebuffers;
            VkCommandPool commandPool;

            std::vector<GenericRenderer*> renderers;
            bool renderersDirty;
    };

}

#endif
