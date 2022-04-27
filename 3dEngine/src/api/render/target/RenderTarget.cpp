#include <algorithm>
#include <cassert>

#include <api/render/target/RenderTarget.h>
#include <api/setup/GraphicService.h>
#include <api/helper/DebugLabelHelper.h>
#include <api/helper/ImageHelper.h>
#include <api/render/GenericRenderer.h>
#include <api/render/target/OffscreenRender.h>
#include <api/render/GenericRendererComparator.h>

namespace urchin {

    RenderTarget::RenderTarget(std::string name, DepthAttachmentType depthAttachmentType) :
            isInitialized(false),
            name(std::move(name)),
            depthAttachmentType(depthAttachmentType),
            renderPass(nullptr),
            renderPassCompatibilityId(0),
            commandPool(nullptr),
            renderersDirty(false) {
        Logger::instance().logInfo("Create render target: " + this->name);
    }

    RenderTarget::~RenderTarget() {
        //Renderers must be destroyed before its render target.
        //Indeed, the renderers' destructor will fail to unlink render target and the renderer.
        assert(renderers.empty());
    }

    std::string RenderTarget::getName() const {
        return name;
    }

    bool RenderTarget::isValidRenderTarget() const {
        return true;
    }

    VkRenderPass RenderTarget::getRenderPass() const {
        return renderPass;
    }

    std::size_t RenderTarget::getRenderPassCompatibilityId() const {
        return renderPassCompatibilityId;
    }

    bool RenderTarget::hasDepthAttachment() const {
        return depthAttachmentType != NO_DEPTH_ATTACHMENT;
    }

    RenderTarget::DepthAttachmentType RenderTarget::getDepthAttachmentType() const {
        return depthAttachmentType;
    }

    /**
     * Provide a depth texture created in another render target
     */
    void RenderTarget::setExternalDepthTexture(const std::shared_ptr<Texture>& externalDepthTexture) {
        assert(!isInitialized);
        if (depthAttachmentType != EXTERNAL_DEPTH_ATTACHMENT) {
            throw std::runtime_error("Can not define an external depth texture on render target: " + getName() + ". Wrong type of depth attachment: " + std::to_string(depthAttachmentType));
        }
        this->externalDepthTexture = externalDepthTexture;
    }

    const std::shared_ptr<Texture>& RenderTarget::getDepthTexture() const {
        if (depthAttachmentType == NO_DEPTH_ATTACHMENT) {
            throw std::runtime_error("Cannot retrieve depth texture on the render target '" + getName() + "' created without a depth attachment");
        } else if (depthAttachmentType == LOCAL_DEPTH_ATTACHMENT) {
            throw std::runtime_error("Cannot retrieve depth texture on the render target '" + getName() + "' created with a local depth attachment");
        } else if (depthAttachmentType == SHARED_DEPTH_ATTACHMENT) {
            return depthTexture;
        } else if (depthAttachmentType == EXTERNAL_DEPTH_ATTACHMENT) {
            return externalDepthTexture;
        }
        throw std::runtime_error("Unknown depth attachment type '" + std::to_string(depthAttachmentType) + "' on render target: " + getName());
    }

    void RenderTarget::addRenderer(GenericRenderer* renderer) {
        #ifdef URCHIN_DEBUG
            assert(&renderer->getRenderTarget() == this);
            for (const auto* r : renderers) {
                assert(r != renderer);
            }
        #endif

        renderers.emplace_back(renderer);
        renderersDirty = true;
    }

    void RenderTarget::removeRenderer(const GenericRenderer* renderer) {
        std::erase(renderers, renderer);
        renderersDirty = true;
    }

    void RenderTarget::notifyRendererEnabled(const GenericRenderer* renderer) {
        if (!renderer->isEnabled()) {
            assert(false);
        }
        renderersDirty = true;
    }

    void RenderTarget::notifyRendererDisabled(const GenericRenderer* renderer) {
        if (renderer->isEnabled()) {
            assert(false);
        }
        renderersDirty = true;
    }

    void RenderTarget::disableAllRenderers() const {
        for (auto& renderer: renderers) {
            if (renderer->isEnabled()) {
                renderer->disableRenderer();
            }
        }
    }

    void RenderTarget::initializeRenderers() {
        for (auto& renderer: renderers) {
            renderer->initialize();
        }
        renderersDirty = true;
    }

    void RenderTarget::cleanupRenderers() const {
        for (auto& renderer: renderers) {
            renderer->cleanup();
        }
    }

    std::span<GenericRenderer* const> RenderTarget::getRenderers() const {
        return renderers;
    }

    bool RenderTarget::hasRenderer() const {
        return std::ranges::any_of(renderers, [](const auto* renderer) {
            return renderer->isEnabled();
        });
    }

    bool RenderTarget::areRenderersDirty() const {
        return renderersDirty;
    }

    VkAttachmentDescription RenderTarget::buildDepthAttachment(VkImageLayout finalLayout) const {
        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = VK_FORMAT_D32_SFLOAT;
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

        if (depthAttachmentType == EXTERNAL_DEPTH_ATTACHMENT) {
            depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
            depthAttachment.initialLayout = finalLayout;
        } else if (depthAttachmentType == LOCAL_DEPTH_ATTACHMENT || depthAttachmentType == SHARED_DEPTH_ATTACHMENT) {
            depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        }
        depthAttachment.finalLayout = finalLayout;

        if (depthAttachmentType == LOCAL_DEPTH_ATTACHMENT) {
            depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        } else if (depthAttachmentType == SHARED_DEPTH_ATTACHMENT || depthAttachmentType == EXTERNAL_DEPTH_ATTACHMENT) {
            depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        } else {
            throw std::runtime_error("Unknown depth attachment type '" + std::to_string(depthAttachmentType) + "' on render target: " + getName());
        }

        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

        return depthAttachment;
    }

    VkAttachmentDescription RenderTarget::buildAttachment(VkFormat format, bool clearOnLoad, bool loadContent, VkImageLayout finalLayout) const {
        assert(!clearOnLoad || !loadContent);
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = format;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        if (clearOnLoad) {
            colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        } else if (loadContent) {
            colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        } else {
            colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        }
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = loadContent ? finalLayout : VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = finalLayout;

        return colorAttachment;
    }

    void RenderTarget::createRenderPass(const VkAttachmentReference& depthAttachmentRef, const std::vector<VkAttachmentReference>& colorAttachmentRefs,
                                        const std::vector<VkAttachmentDescription>& attachments) {

        //See https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/vkspec.html#renderpass-compatibility for details
        renderPassCompatibilityId = 0;
        if (hasDepthAttachment()) {
            HashUtil::combine(renderPassCompatibilityId, depthAttachmentRef.attachment);
        }
        for (auto& colorAttachmentRef : colorAttachmentRefs) {
            HashUtil::combine(renderPassCompatibilityId, colorAttachmentRef.attachment);
        }
        for (auto& attachment : attachments) {
            HashUtil::combine(renderPassCompatibilityId, attachment.format, attachment.samples, attachment.flags);
        }

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = (uint32_t)colorAttachmentRefs.size();
        subpass.pColorAttachments = colorAttachmentRefs.data(); //position determine index in the shader (layout(location = 0) out vec4 outColor)
        subpass.pDepthStencilAttachment = hasDepthAttachment() ? &depthAttachmentRef : nullptr;
        subpass.inputAttachmentCount = 0;

        VkSubpassDependency dependency{};
        //VK_SUBPASS_EXTERNAL reference the sub-pass of the previous render pass:
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        //Index of the current sub-pass. Always 0 because this engine does not have multiple sub-passes:
        dependency.dstSubpass = 0;
        //Before move on to the current sub-pass, the previous sub-pass must have finish the defined stages in this variable:
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        //The current sub-pass can be executed until the specified stage and then must wait the previous sub-pass reach the stage specified in 'srcStageMask':
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        //All memory access type needed by the previous sub-pass (Allow the GPU to better handle memory cache. Example: if write is done in src, the dst must refresh the cache to read)
        dependency.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT;
        //All memory access type needed by the current sub-pass (Allow the GPU to better handle memory cache. Example: if write is done in src, the dst must refresh the cache to read)
        dependency.dstAccessMask = VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT;

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = (uint32_t)attachments.size();
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        VkResult result = vkCreateRenderPass(GraphicService::instance().getDevices().getLogicalDevice(), &renderPassInfo, nullptr, &renderPass);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create render pass with error code '" + std::to_string(result) + "' on render target: " + getName());
        }

        DebugLabelHelper::nameObject(DebugLabelHelper::RENDER_PASS, renderPass, name);
    }

    void RenderTarget::destroyRenderPass() {
        vkDestroyRenderPass(GraphicService::instance().getDevices().getLogicalDevice(), renderPass, nullptr);
    }

    void RenderTarget::createDepthResources() {
        if (hasDepthAttachment()) {
            if (depthAttachmentType == EXTERNAL_DEPTH_ATTACHMENT) {
                if (!externalDepthTexture) {
                    throw std::runtime_error("An external depth texture is required on render target " + getName());
                } else if (getLayer() != externalDepthTexture->getLayer()) {
                    throw std::runtime_error("The external depth texture has " + std::to_string(externalDepthTexture->getLayer()) + " layer but " + std::to_string(getLayer()) + " are required on render target " + getName());
                }
                depthTexture = externalDepthTexture;
            } else {
                if (getLayer() == 1) {
                    depthTexture = Texture::build(name + " - depth", getWidth(), getHeight(), TextureFormat::DEPTH_32_FLOAT, nullptr);
                } else {
                    depthTexture = Texture::buildArray(name + " - depth", getWidth(), getHeight(), getLayer(), TextureFormat::DEPTH_32_FLOAT, nullptr);
                }
                depthTexture->enableTextureWriting();
                depthTexture->initialize();
            }
        }
    }

    void RenderTarget::destroyDepthResources() {
        if (depthTexture) {
            depthTexture = nullptr;
        }
    }

    void RenderTarget::addNewFrameBuffer(std::span<VkImageView> attachments) {
        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass; //render pass must have the same number and type of attachments as the framebufferInfo
        framebufferInfo.attachmentCount = (uint32_t)attachments.size();
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = getWidth();
        framebufferInfo.height = getHeight();
        framebufferInfo.layers = getLayer();

        framebuffers.resize(framebuffers.size() + 1, nullptr);
        VkResult result = vkCreateFramebuffer(GraphicService::instance().getDevices().getLogicalDevice(), &framebufferInfo, nullptr, &framebuffers[framebuffers.size() - 1]);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create framebuffer with error code '" + std::to_string(result) + "' on render target: " + getName());
        }
    }

    void RenderTarget::destroyFramebuffers() {
        for (auto framebuffer : framebuffers) {
            vkDestroyFramebuffer(GraphicService::instance().getDevices().getLogicalDevice(), framebuffer, nullptr);
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

        VkResult resultCommandBuffers = vkAllocateCommandBuffers(GraphicService::instance().getDevices().getLogicalDevice(), &allocInfo, commandBuffers.data());
        if (resultCommandBuffers != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffers with error code '" + std::to_string(resultCommandBuffers) + "' on render target: " + getName());
        }

        for (std::size_t cmdBufferIndex = 0; cmdBufferIndex < commandBuffers.size(); ++cmdBufferIndex) {
            DebugLabelHelper::nameObject(DebugLabelHelper::COMMAND_BUFFER, commandBuffers[cmdBufferIndex], getName() + "_" + std::to_string(cmdBufferIndex));
        }
    }

    void RenderTarget::createCommandPool() {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = GraphicService::instance().getQueues().getGraphicsQueueFamily();
        poolInfo.flags = 0;

        VkResult result = vkCreateCommandPool(GraphicService::instance().getDevices().getLogicalDevice(), &poolInfo, nullptr, &commandPool);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create command pool with error code '" + std::to_string(result) + "' on render target: " + getName());
        }
    }

    void RenderTarget::destroyCommandBuffersAndPool() {
        vkDestroyCommandPool(GraphicService::instance().getDevices().getLogicalDevice(), commandPool, nullptr);
    }

    std::span<OffscreenRender*> RenderTarget::getRenderDependencies() const {
        renderDependencies.clear();
        for (auto& renderer : renderers) {
            if (renderer->isEnabled()) {
                const std::span<OffscreenRender*>& renderTextureWriter = renderer->getTexturesWriter();
                renderDependencies.insert(renderDependencies.end(), renderTextureWriter.begin(), renderTextureWriter.end());
            }
        }
        if (externalDepthTexture) {
            assert(externalDepthTexture->getLastTextureWriter());
            renderDependencies.push_back(externalDepthTexture->getLastTextureWriter());
        }

        VectorUtil::removeDuplicates(renderDependencies);
        return renderDependencies;
    }

    void RenderTarget::configureWaitSemaphore(std::uint64_t frameIndex, VkSubmitInfo& submitInfo, std::optional<WaitSemaphore> additionalSemaphore) const {
        queueSubmitWaitSemaphores.clear();
        queueSubmitWaitStages.clear();

        if (additionalSemaphore.has_value()) {
            queueSubmitWaitSemaphores.emplace_back(additionalSemaphore->waitSemaphore);
            queueSubmitWaitStages.emplace_back(additionalSemaphore->waitDstStageMask);
        }

        for (OffscreenRender* renderDependency : getRenderDependencies()) {
            VkSemaphore waitSemaphore = renderDependency->popSubmitSemaphore(frameIndex);
            if (waitSemaphore) {
                queueSubmitWaitSemaphores.emplace_back(waitSemaphore);
                queueSubmitWaitStages.emplace_back(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT /* for depth attachment */);
            }
        }

        submitInfo.waitSemaphoreCount = (uint32_t)queueSubmitWaitSemaphores.size();
        submitInfo.pWaitSemaphores = queueSubmitWaitSemaphores.data();
        submitInfo.pWaitDstStageMask = queueSubmitWaitStages.data();
    }

    void RenderTarget::updateGraphicData(uint32_t frameIndex) const {
        ScopeProfiler sp(Profiler::graphic(), "upShaderData");

        for (auto& renderer : renderers) {
            if (renderer->isEnabled()) {
                renderer->updateGraphicData(frameIndex);
            }
        }
    }

    void RenderTarget::updateCommandBuffers(uint32_t frameIndex, const std::vector<VkClearValue>& clearValues) {
        ScopeProfiler sp(Profiler::graphic(), "upCmdBufTarget");

        if (needCommandBufferRefresh(frameIndex)) {
            std::ranges::sort(renderers, GenericRendererComparator());

            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = renderPass;
            renderPassInfo.framebuffer = nullptr;
            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = {getWidth(), getHeight()};
            renderPassInfo.clearValueCount = (uint32_t) clearValues.size();
            renderPassInfo.pClearValues = clearValues.data();

            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = 0;
            beginInfo.pInheritanceInfo = nullptr; //only relevant for secondary command buffers

            waitCommandBuffersIdle();
            VkResult resultResetCmdPool = vkResetCommandPool(GraphicService::instance().getDevices().getLogicalDevice(), commandPool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
            if (resultResetCmdPool != VK_SUCCESS) {
                throw std::runtime_error("Failed to reset command pool with error code '" + std::to_string(resultResetCmdPool) + "' on render target: " + getName());
            }

            VkResult resultCmdBuffer = vkBeginCommandBuffer(commandBuffers[frameIndex], &beginInfo);
            {
                if (resultCmdBuffer != VK_SUCCESS) {
                    throw std::runtime_error("Failed to begin recording command buffer with error code '" + std::to_string(resultCmdBuffer) + "' on render target: " + getName());
                }

                renderPassInfo.framebuffer = framebuffers[frameIndex];

                DebugLabelHelper::beginDebugRegion(commandBuffers[frameIndex], name, Vector4<float>(0.9f, 1.0f, 0.8f, 1.0f));
                vkCmdBeginRenderPass(commandBuffers[frameIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
                {
                    std::size_t boundPipelineId = 0;
                    for (auto& renderer: renderers) {
                        if (renderer->isEnabled()) {
                            boundPipelineId = renderer->updateCommandBuffer(commandBuffers[frameIndex], frameIndex, boundPipelineId);
                        }
                    }
                }
                vkCmdEndRenderPass(commandBuffers[frameIndex]);
                DebugLabelHelper::endDebugRegion(commandBuffers[frameIndex]);
            }
            VkResult resultEndCmdBuffer = vkEndCommandBuffer(commandBuffers[frameIndex]);
            if (resultEndCmdBuffer != VK_SUCCESS) {
                throw std::runtime_error("Failed to record command buffer with error code '" + std::to_string(resultEndCmdBuffer) + "' on render target: " + getName());
            }

            renderersDirty = false;
        }
    }

}
