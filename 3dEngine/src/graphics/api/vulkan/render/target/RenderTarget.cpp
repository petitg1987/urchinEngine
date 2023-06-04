#include <algorithm>
#include <cassert>
#include <vulkan/vk_enum_string_helper.h>

#include <graphics/api/vulkan/render/target/RenderTarget.h>
#include <graphics/api/vulkan/setup/GraphicsSetupService.h>
#include <graphics/api/vulkan/helper/DebugLabelHelper.h>
#include <graphics/api/vulkan/helper/ImageHelper.h>
#include <graphics/api/vulkan/render/PipelineProcessor.h>
#include <graphics/api/vulkan/render/target/OffscreenRender.h>
#include <graphics/render/PipelineProcessorComparator.h>

namespace urchin {

    RenderTarget::RenderTarget(std::string name, DepthAttachmentType depthAttachmentType) :
            isInitialized(false),
            name(std::move(name)),
            depthAttachmentType(depthAttachmentType),
            renderPass(nullptr),
            renderPassCompatibilityId(0),
            commandPool(nullptr),
            processorsDirty(false) {
        Logger::instance().logInfo("Create render target: " + this->name);
    }

    RenderTarget::~RenderTarget() {
        //Processors must be destroyed before its render target.
        //Indeed, the processors' destructor will fail to unlink render target and the processor.
        assert(processors.empty());
    }

    std::string RenderTarget::getName() const {
        return name;
    }

    bool RenderTarget::isValidRenderTarget() const {
        return true;
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

    void RenderTarget::addProcessor(PipelineProcessor* processor) {
        #ifdef URCHIN_DEBUG
            assert(&processor->getRenderTarget() == this);
            assert(processors.empty() || processors[0]->getPipelineType() == processor->getPipelineType());
            for (const auto* p : processors) {
                assert(p != processor);
            }
        #endif

        processors.emplace_back(processor);
        processorsDirty = true;
    }

    void RenderTarget::removeProcessor(const PipelineProcessor* processor) {
        std::erase(processors, processor);
        processorsDirty = true;
    }

    void RenderTarget::notifyProcessorEnabled(const PipelineProcessor* processor) {
        if (!processor->isEnabled()) {
            assert(false);
        }
        processorsDirty = true;
    }

    void RenderTarget::notifyProcessorDisabled(const PipelineProcessor* processor) {
        if (processor->isEnabled()) {
            assert(false);
        }
        processorsDirty = true;
    }

    void RenderTarget::disableAllProcessors() const {
        for (auto& processor : processors) {
            if (processor->isEnabled()) {
                processor->disableRenderer();
            }
        }
    }

    void RenderTarget::initializeProcessors() {
        for (auto& processor : processors) {
            processor->initialize();
        }
        processorsDirty = true;
    }

    void RenderTarget::cleanupProcessors() const {
        for (auto& processor : processors) {
            processor->cleanup();
        }
    }

    std::span<PipelineProcessor* const> RenderTarget::getProcessors() const {
        return processors;
    }

    bool RenderTarget::hasProcessor() const {
        return std::ranges::any_of(processors, [](const auto* renderer) {
            return renderer->isEnabled();
        });
    }

    bool RenderTarget::areProcessorsDirty() const {
        return processorsDirty;
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
        subpass.pColorAttachments = colorAttachmentRefs.empty() ? nullptr : colorAttachmentRefs.data(); //position determine index in the shader (layout(location = 0) out vec4 outColor)
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
        renderPassInfo.pAttachments = attachments.empty() ? nullptr : attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        VkResult result = vkCreateRenderPass(GraphicsSetupService::instance().getDevices().getLogicalDevice(), &renderPassInfo, nullptr, &renderPass);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create render pass with error code '" + std::string(string_VkResult(result)) + "' on render target: " + getName());
        }

        DebugLabelHelper::nameObject(DebugLabelHelper::RENDER_PASS, renderPass, name);
    }

    void RenderTarget::destroyRenderPass() {
        vkDestroyRenderPass(GraphicsSetupService::instance().getDevices().getLogicalDevice(), renderPass, nullptr);
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
                    depthTexture = Texture::build(name + " - depth", getWidth(), getHeight(), TextureFormat::DEPTH_32_FLOAT);
                } else {
                    depthTexture = Texture::buildArray(name + " - depth", getWidth(), getHeight(), getLayer(), TextureFormat::DEPTH_32_FLOAT);
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
        framebufferInfo.pAttachments = attachments.empty() ? nullptr : attachments.data();
        framebufferInfo.width = getWidth();
        framebufferInfo.height = getHeight();
        framebufferInfo.layers = getLayer();

        framebuffers.resize(framebuffers.size() + 1, nullptr);
        VkResult result = vkCreateFramebuffer(GraphicsSetupService::instance().getDevices().getLogicalDevice(), &framebufferInfo, nullptr, &framebuffers[framebuffers.size() - 1]);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create framebuffer with error code '" + std::string(string_VkResult(result)) + "' on render target: " + getName());
        }
    }

    void RenderTarget::destroyFramebuffers() {
        for (auto framebuffer : framebuffers) {
            vkDestroyFramebuffer(GraphicsSetupService::instance().getDevices().getLogicalDevice(), framebuffer, nullptr);
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

        VkResult resultCommandBuffers = vkAllocateCommandBuffers(GraphicsSetupService::instance().getDevices().getLogicalDevice(), &allocInfo, commandBuffers.data());
        if (resultCommandBuffers != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffers with error code '" + std::string(string_VkResult(resultCommandBuffers)) + "' on render target: " + getName());
        }

        for (std::size_t cmdBufferIndex = 0; cmdBufferIndex < commandBuffers.size(); ++cmdBufferIndex) {
            DebugLabelHelper::nameObject(DebugLabelHelper::COMMAND_BUFFER, commandBuffers[cmdBufferIndex], getName() + "_" + std::to_string(cmdBufferIndex));
        }
    }

    void RenderTarget::createCommandPool() {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = GraphicsSetupService::instance().getQueues().getGraphicsAndComputeQueueFamily();
        poolInfo.flags = 0;

        VkResult result = vkCreateCommandPool(GraphicsSetupService::instance().getDevices().getLogicalDevice(), &poolInfo, nullptr, &commandPool);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create command pool with error code '" + std::string(string_VkResult(result)) + "' on render target: " + getName());
        }
    }

    void RenderTarget::destroyCommandBuffersAndPool() {
        vkDestroyCommandPool(GraphicsSetupService::instance().getDevices().getLogicalDevice(), commandPool, nullptr);
    }

    VkRenderPass RenderTarget::getRenderPass() const {
        return renderPass;
    }

    std::span<OffscreenRender*> RenderTarget::getRenderDependencies() const {
        renderDependencies.clear(); //TODO rename render to processor ?
        for (auto& processor : processors) {
            if (processor->isEnabled()) {
                const std::span<OffscreenRender*>& renderTextureWriter = processor->getTexturesWriter();
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

    void RenderTarget::configureWaitSemaphore(std::uint32_t frameIndex, VkSubmitInfo& submitInfo, std::optional<WaitSemaphore> additionalSemaphore) const {
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
        submitInfo.pWaitSemaphores = queueSubmitWaitSemaphores.empty() ? nullptr : queueSubmitWaitSemaphores.data();
        submitInfo.pWaitDstStageMask = queueSubmitWaitStages.data();
    }

    void RenderTarget::updateGraphicData(uint32_t frameIndex) const {
        ScopeProfiler sp(Profiler::graphic(), "upShaderData");

        for (auto& processor : processors) {
            if (processor->isEnabled()) {
                processor->updateGraphicData(frameIndex);
            }
        }
    }

    void RenderTarget::updateCommandBuffers(uint32_t frameIndex, const std::vector<VkClearValue>& clearValues) {
        ScopeProfiler sp(Profiler::graphic(), "upCmdBufTarget");

        if (needCommandBufferRefresh(frameIndex)) {
            sortedEnabledProcessors.clear();
            for (PipelineProcessor* processor: processors) {
                if (processor->isEnabled()) {
                    sortedEnabledProcessors.emplace_back(processor);
                }
            }
            std::ranges::sort(sortedEnabledProcessors, PipelineProcessorComparator());

            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = renderPass;
            renderPassInfo.framebuffer = nullptr;
            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = {getWidth(), getHeight()};
            renderPassInfo.clearValueCount = (uint32_t)clearValues.size();
            renderPassInfo.pClearValues = clearValues.empty() ? nullptr : clearValues.data();

            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = 0;
            beginInfo.pInheritanceInfo = nullptr; //only relevant for secondary command buffers

            waitCommandBuffersIdle();
            VkResult resultResetCmdPool = vkResetCommandPool(GraphicsSetupService::instance().getDevices().getLogicalDevice(), commandPool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
            if (resultResetCmdPool != VK_SUCCESS) {
                throw std::runtime_error("Failed to reset command pool with error code '" + std::string(string_VkResult(resultResetCmdPool)) + "' on render target: " + getName());
            }

            VkResult resultCmdBuffer = vkBeginCommandBuffer(commandBuffers[frameIndex], &beginInfo);
            {
                if (resultCmdBuffer != VK_SUCCESS) {
                    throw std::runtime_error("Failed to begin recording command buffer with error code '" + std::string(string_VkResult(resultCmdBuffer)) + "' on render target: " + getName());
                }

                renderPassInfo.framebuffer = framebuffers[frameIndex];

                DebugLabelHelper::beginDebugRegion(commandBuffers[frameIndex], name, Vector4<float>(0.9f, 1.0f, 0.8f, 1.0f));
                if (sortedEnabledProcessors.empty() || sortedEnabledProcessors[0]->getPipelineType() == PipelineType::GRAPHICS) {
                    vkCmdBeginRenderPass(commandBuffers[frameIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
                }
                std::size_t boundPipelineId = 0;
                for (PipelineProcessor* processor: sortedEnabledProcessors) {
                    boundPipelineId = processor->updateCommandBuffer(commandBuffers[frameIndex], frameIndex, boundPipelineId);
                }
                if (sortedEnabledProcessors.empty() || sortedEnabledProcessors[0]->getPipelineType() == PipelineType::GRAPHICS) {
                    vkCmdEndRenderPass(commandBuffers[frameIndex]);
                }
                DebugLabelHelper::endDebugRegion(commandBuffers[frameIndex]);
            }
            VkResult resultEndCmdBuffer = vkEndCommandBuffer(commandBuffers[frameIndex]);
            if (resultEndCmdBuffer != VK_SUCCESS) {
                throw std::runtime_error("Failed to record command buffer with error code '" + std::string(string_VkResult(resultEndCmdBuffer)) + "' on render target: " + getName());
            }

            processorsDirty = false;
        }
    }

}
