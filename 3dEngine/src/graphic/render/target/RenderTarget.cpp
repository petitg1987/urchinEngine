#include <algorithm>

#include "RenderTarget.h"
#include "graphic/setup/GraphicService.h"
#include "graphic/helper/ImageHelper.h"
#include "graphic/render/GenericRenderer.h"

namespace urchin {

    RenderTarget::RenderTarget(DepthAttachmentType depthAttachmentType) :
            renderPass(nullptr),
            depthAttachmentType(depthAttachmentType),
            commandPool(nullptr),
            renderersDirty(false) {

    }

    VkRenderPass RenderTarget::getRenderPass() const {
        return renderPass;
    }

    bool RenderTarget::hasDepthAttachment() const {
        return depthAttachmentType != NO_DEPTH_ATTACHMENT;
    }

    bool RenderTarget::isReadableDepthAttachment() const {
        return depthAttachmentType == READ_WRITE_DEPTH_ATTACHMENT;
    }

    const std::shared_ptr<Texture>& RenderTarget::getDepthTexture() const {
        if (!isReadableDepthAttachment()) {
            throw std::runtime_error("Cannot retrieve depth texture on a render target created without a readable depth attachment");
        }
        return depthTexture;
    }

    void RenderTarget::addRenderer(GenericRenderer* renderer) {
        renderers.emplace_back(renderer);
        renderersDirty = true;
    }

    void RenderTarget::removeRenderer(GenericRenderer* renderer) {
        renderers.erase(std::remove(renderers.begin(), renderers.end(), renderer), renderers.end());
        renderersDirty = true;
    }

    void RenderTarget::clearRenderers() {
        renderers.clear();
        renderersDirty = true;
    }

    bool RenderTarget::needCommandBuffersRefresh() const {
        return renderersDirty || std::any_of(renderers.begin(), renderers.end(), [](const auto& renderer){return renderer->isDrawCommandDirty();});
    }

    void RenderTarget::initializeRenderers() {
        for (auto& renderer: renderers) {
            renderer->initialize();
        }
    }

    void RenderTarget::cleanupRenderers() {
        for (auto& renderer: renderers) {
            renderer->cleanup();
        }
    }

    VkAttachmentDescription RenderTarget::buildDepthAttachment(VkImageLayout finalLayout) const {
        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = VK_FORMAT_D32_SFLOAT;
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = isReadableDepthAttachment() ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = finalLayout;
        return depthAttachment;
    }

    VkAttachmentDescription RenderTarget::buildAttachment(VkFormat format, VkImageLayout finalLayout) {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = format;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; //clear color attachment at load
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = finalLayout;
        return colorAttachment;
    }

    void RenderTarget::createRenderPass(const VkAttachmentReference &depthAttachmentRef, const std::vector<VkAttachmentReference>& colorAttachmentRefs,
                                        const std::vector<VkAttachmentDescription>& attachments) {
        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = (uint32_t)colorAttachmentRefs.size();
        subpass.pColorAttachments = colorAttachmentRefs.data(); //position determine index in the shader (layout(location = 0) out vec4 outColor)
        subpass.pDepthStencilAttachment = hasDepthAttachment() ? &depthAttachmentRef : nullptr;
        subpass.inputAttachmentCount = 0;

        VkSubpassDependency dependency{}; //define dependency between build-in sub-pass (VK_SUBPASS_EXTERNAL) and first sub-pass index 0
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0; //index of the sub-pass
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT /* for depth attachment */;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT /* for depth attachment */;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = (uint32_t)attachments.size();
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        VkResult result = vkCreateRenderPass(GraphicService::instance()->getDevices().getLogicalDevice(), &renderPassInfo, nullptr, &renderPass);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create render pass with error code: " + std::to_string(result));
        }
    }

    void RenderTarget::destroyRenderPass() {
        vkDestroyRenderPass(GraphicService::instance()->getDevices().getLogicalDevice(), renderPass, nullptr);
    }

    void RenderTarget::createDepthResources() {
        if (hasDepthAttachment()) {
            depthTexture = Texture::build(getWidth(), getHeight(), TextureFormat::DEPTH_32_FLOAT, nullptr);
            depthTexture->initialize();
        }
    }

    void RenderTarget::destroyDepthResources() {
        if (depthTexture) {
            depthTexture = nullptr;
        }
    }

    void RenderTarget::addNewFrameBuffer(const std::vector<VkImageView>& attachments) {
        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass; //render pass must have same number and type of attachments as the framebufferInfo
        framebufferInfo.attachmentCount = (uint32_t)attachments.size();
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = getWidth();
        framebufferInfo.height = getHeight();
        framebufferInfo.layers = 1;

        framebuffers.resize(framebuffers.size() + 1, nullptr);
        VkResult result = vkCreateFramebuffer(GraphicService::instance()->getDevices().getLogicalDevice(), &framebufferInfo, nullptr, &framebuffers[framebuffers.size() - 1]);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create framebuffer with error code: " + std::to_string(result));
        }
    }

    void RenderTarget::destroyFramebuffers() {
        for (auto framebuffer : framebuffers) {
            vkDestroyFramebuffer(GraphicService::instance()->getDevices().getLogicalDevice(), framebuffer, nullptr);
        }
        framebuffers.clear();
    }

    void RenderTarget::createCommandBuffers() {
        assert(!framebuffers.empty());

        commandBuffers.resize(framebuffers.size());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

        VkResult resultCommandBuffers = vkAllocateCommandBuffers(GraphicService::instance()->getDevices().getLogicalDevice(), &allocInfo, commandBuffers.data());
        if (resultCommandBuffers != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffers with error code: " + std::to_string(resultCommandBuffers));
        }
    }

    void RenderTarget::createCommandPool() {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = GraphicService::instance()->getQueues().getGraphicsQueueFamily();
        poolInfo.flags = 0;

        VkResult result = vkCreateCommandPool(GraphicService::instance()->getDevices().getLogicalDevice(), &poolInfo, nullptr, &commandPool);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create command pool with error code: " + std::to_string(result));
        }
    }

    void RenderTarget::destroyCommandBuffersAndPool() {
        vkDestroyCommandPool(GraphicService::instance()->getDevices().getLogicalDevice(), commandPool, nullptr);
    }

    void RenderTarget::updateGraphicData(uint32_t frameIndex) {
        for (auto &renderer : renderers) {
            renderer->updateGraphicData(frameIndex);
        }
    }

    void RenderTarget::updateCommandBuffers(const std::vector<VkClearValue>& clearValues) {
        ScopeProfiler sp(Profiler::graphic(), "upCmdBuffers");
        if(!needCommandBuffersRefresh()) {
            return;
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = nullptr;
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = {getWidth(), getHeight()};
        renderPassInfo.clearValueCount = (uint32_t)clearValues.size();
        renderPassInfo.pClearValues = clearValues.data();

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;
        beginInfo.pInheritanceInfo = nullptr; //only relevant for secondary command buffers

        waitCommandBuffersIdle();
        vkResetCommandPool(GraphicService::instance()->getDevices().getLogicalDevice(), commandPool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);

        for (std::size_t i = 0; i < commandBuffers.size(); i++) {
            VkResult resultCommandBuffer = vkBeginCommandBuffer(commandBuffers[i], &beginInfo);
            if (resultCommandBuffer != VK_SUCCESS) {
                throw std::runtime_error("Failed to begin recording command buffer with error code: " + std::to_string(resultCommandBuffer));
            }

            renderPassInfo.framebuffer = framebuffers[i];

            vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
            for (auto &renderer : renderers) {
                renderer->updateCommandBuffer(commandBuffers[i], i);
            }
            vkCmdEndRenderPass(commandBuffers[i]);

            VkResult resultEndCommandBuffer = vkEndCommandBuffer(commandBuffers[i]);
            if (resultEndCommandBuffer != VK_SUCCESS) {
                throw std::runtime_error("Failed to record command buffer with error code: " + std::to_string(resultEndCommandBuffer));
            }
        }

        renderersDirty = false;
    }

}
