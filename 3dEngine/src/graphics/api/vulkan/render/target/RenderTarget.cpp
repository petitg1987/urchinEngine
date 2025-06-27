#include <algorithm>
#include <cassert>
#include <libs/vkenum/vk_enum.h>

#include <graphics/api/vulkan/render/target/RenderTarget.h>
#include <graphics/api/vulkan/setup/GraphicsSetupService.h>
#include <graphics/api/vulkan/helper/DebugLabelHelper.h>
#include <graphics/api/vulkan/render/PipelineProcessor.h>
#include <graphics/api/vulkan/render/target/OffscreenRender.h>
#include <graphics/render/PipelineProcessorComparator.h>

namespace urchin {

    RenderTarget::RenderTarget(std::string name, bool isTestMode, DepthAttachmentType depthAttachmentType) :
            bIsInitialized(false),
            bIsTestMode(isTestMode),
            name(std::move(name)),
            depthAttachmentType(depthAttachmentType),
            renderPass(nullptr),
            renderPassCompatibilityId(0),
            commandPool(nullptr),
            copiersDirty(false),
            processorsDirty(false),
            framebufferDirty(false) {
        Logger::instance().logInfo("Creating a render target: " + this->name);
    }

    RenderTarget::~RenderTarget() {
        //Processors must be destroyed before its render target.
        //Indeed, the processors' destructor will fail to unlink render target and the processor.
        assert(processors.empty());
    }

    bool RenderTarget::isInitialized() const {
        return bIsInitialized;
    }

    void RenderTarget::setInitialized(bool isInitialized) {
        this->bIsInitialized = isInitialized;
    }

    std::string RenderTarget::getName() const {
        return name;
    }

    bool RenderTarget::isTestMode() const {
        return bIsTestMode;
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
        assert(!bIsInitialized);
        if (depthAttachmentType != EXTERNAL_DEPTH_ATTACHMENT) {
            throw std::runtime_error("Can not define an external depth texture on render target: " + getName() + ". Wrong type of depth attachment: " + std::to_string(depthAttachmentType));
        }
        this->externalDepthTexture = externalDepthTexture;
    }

    bool RenderTarget::hasDepthTexture() const {
        return depthTexture != nullptr;
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

    void RenderTarget::addPreRenderTextureCopier(TextureCopier textureCopier) {
        preRenderTextureCopier.emplace_back(textureCopier);
        preRenderTextureCopier.back().initialize();

        copiersDirty = true;
    }

    void RenderTarget::removeAllPreRenderTextureCopiers() {
        preRenderTextureCopier.clear();
    }

    void RenderTarget::addProcessor(PipelineProcessor* processor) {
        #ifdef URCHIN_DEBUG
            assert(&processor->getRenderTarget() == this);
            assert(isTestMode() || (processor->isGraphicsProcessor() || !hasDepthAttachment()));
            assert(processors.empty() || (processors[0]->isGraphicsProcessor() == processor->isGraphicsProcessor() && processors[0]->isComputeProcessor() == processor->isComputeProcessor()));
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

    bool RenderTarget::couldHaveGraphicsProcessors() const {
        return processors.empty() || processors[0]->isGraphicsProcessor();
    }

    bool RenderTarget::hasGraphicsProcessors() const {
        return !processors.empty() && processors[0]->isGraphicsProcessor();
    }

    bool RenderTarget::hasComputeProcessors() const {
        return !processors.empty() && processors[0]->isComputeProcessor();
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

    bool RenderTarget::needCommandBufferRefresh() const {
        return processorsDirty || copiersDirty || framebufferDirty;
    }

    VkAttachmentDescription2 RenderTarget::buildDepthAttachment(VkFormat format, VkImageLayout finalLayout) const {
        VkAttachmentDescription2 depthAttachment{};
        depthAttachment.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
        depthAttachment.format = format;
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

    VkAttachmentDescription2 RenderTarget::buildAttachment(VkFormat format, bool clearOnLoad, bool loadContent, VkImageLayout finalLayout) const {
        assert(!clearOnLoad || !loadContent);
        VkAttachmentDescription2 colorAttachment{};
        colorAttachment.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
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

    void RenderTarget::createRenderPass(const VkAttachmentReference2& depthAttachmentRef, const std::vector<VkAttachmentReference2>& colorAttachmentRefs,
                                        const std::vector<VkAttachmentDescription2>& attachments) {

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

        VkSubpassDescription2 subpass{};
        subpass.sType = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2;
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = (uint32_t)colorAttachmentRefs.size();
        subpass.pColorAttachments = colorAttachmentRefs.empty() ? nullptr : colorAttachmentRefs.data(); //position determine index in the shader (layout(location = 0) out vec4 outColor)
        subpass.pDepthStencilAttachment = hasDepthAttachment() ? &depthAttachmentRef : nullptr;
        subpass.inputAttachmentCount = 0;

        VkMemoryBarrier2 memoryBarrier{};
        memoryBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2_KHR;
        memoryBarrier.pNext = nullptr;
        //Before move on to the current sub-pass, the previous sub-pass must have finished the defined stages in this variable:
        memoryBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT;
        //The current sub-pass can be executed until the specified stage and then must wait the previous sub-pass reach the stage specified in 'srcStageMask':
        memoryBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT;
        //All memory access type needed by the previous sub-pass (Allow the GPU to better handle memory cache. Example: if write is done in src, the dst must refresh the cache to read)
        memoryBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;
        //All memory access type needed by the current sub-pass (Allow the GPU to better handle memory cache. Example: if write is done in src, the dst must refresh the cache to read)
        memoryBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

        VkSubpassDependency2 dependency{};
        dependency.sType = VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2;
        dependency.pNext = &memoryBarrier;
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL; //VK_SUBPASS_EXTERNAL reference the sub-pass of the previous render pass
        dependency.dstSubpass = 0; //Index of the current sub-pass. Always 0 because this engine does not have multiple sub-passes
        dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        VkRenderPassCreateInfo2 renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2;
        renderPassInfo.attachmentCount = (uint32_t)attachments.size();
        renderPassInfo.pAttachments = attachments.empty() ? nullptr : attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        VkResult result = vkCreateRenderPass2(GraphicsSetupService::instance().getDevices().getLogicalDevice(), &renderPassInfo, nullptr, &renderPass);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create render pass with error code '" + std::string(string_VkResult(result)) + "' on render target: " + getName());
        }

        DebugLabelHelper::nameObject(DebugLabelHelper::RENDER_PASS, renderPass, name);
    }

    void RenderTarget::destroyRenderPass() {
        if (renderPass) {
            vkDestroyRenderPass(GraphicsSetupService::instance().getDevices().getLogicalDevice(), renderPass, nullptr);
            renderPass = nullptr;
        }
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
                if (isArrayOutput()) {
                    depthTexture = Texture::buildArray(name + " - depth", getWidth(), getHeight(), getLayer(), TextureFormat::DEPTH_32_FLOAT);
                } else {
                    depthTexture = Texture::build(name + " - depth", getWidth(), getHeight(), TextureFormat::DEPTH_32_FLOAT);
                }
                depthTexture->enableTextureWriting(OutputUsage::GRAPHICS);
                if (!isTestMode()) {
                    depthTexture->initialize();
                }
            }
        }
    }

    void RenderTarget::destroyDepthResources() {
        if (depthTexture) {
            depthTexture = nullptr;
        }
    }

    void RenderTarget::addFramebuffers(const std::vector<std::vector<VkImageView>>& attachments) {
        if (!attachments.empty()) {
            framebuffers.resize(framebuffers.size() + 1);
            std::size_t frameIndex = framebuffers.size() - 1;

            framebuffers[frameIndex].resize(getLayer(), nullptr);
            for (std::size_t layerIndex = 0; layerIndex < attachments.size(); ++layerIndex) {
                if (attachments[layerIndex].empty()) {
                    continue;
                }

                VkFramebufferCreateInfo framebufferInfo{};
                framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                framebufferInfo.renderPass = renderPass; //render pass must have the same number and type of attachments as the framebufferInfo
                framebufferInfo.attachmentCount = (uint32_t) attachments[layerIndex].size();
                framebufferInfo.pAttachments = attachments[layerIndex].data();
                framebufferInfo.width = getWidth();
                framebufferInfo.height = getHeight();
                framebufferInfo.layers = 1;

                VkResult result = vkCreateFramebuffer(GraphicsSetupService::instance().getDevices().getLogicalDevice(), &framebufferInfo, nullptr, &framebuffers[frameIndex][layerIndex]);
                if (result != VK_SUCCESS) {
                    throw std::runtime_error("Failed to create framebuffer with error code '" + std::string(string_VkResult(result)) + "' on render target: " + getName());
                }
            }
            framebufferDirty = true;
        }
    }

    void RenderTarget::destroyFramebuffers() {
        for (const auto& framebufferLayers : framebuffers) {
            for (const auto& framebuffer : framebufferLayers) {
                vkDestroyFramebuffer(GraphicsSetupService::instance().getDevices().getLogicalDevice(), framebuffer, nullptr);
            }
        }
        framebuffers.clear();
        framebufferDirty = true;
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

    void RenderTarget::createCommandBuffers() {
        commandBuffers.resize(getNumFramebuffer());

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

    void RenderTarget::destroyCommandBuffersAndPool() const {
        vkDestroyCommandPool(GraphicsSetupService::instance().getDevices().getLogicalDevice(), commandPool, nullptr);
    }

    VkRenderPass RenderTarget::getRenderPass() const {
        return renderPass;
    }

    VkCommandBuffer RenderTarget::getCommandBuffer(std::size_t cmdBufferIndex) const {
        return commandBuffers[cmdBufferIndex];
    }

    std::span<OffscreenRender*> RenderTarget::getOffscreenRenderDependencies() const {
        offscreenRenderDependencies.clear();
        for (auto& processor : processors) {
            if (processor->isEnabled()) {
                const std::span<OffscreenRender*>& renderTextureWriter = processor->getTexturesWriter();
                offscreenRenderDependencies.insert(offscreenRenderDependencies.end(), renderTextureWriter.begin(), renderTextureWriter.end());
            }
        }
        if (externalDepthTexture) {
            assert(externalDepthTexture->getLastTextureWriter());
            offscreenRenderDependencies.push_back(externalDepthTexture->getLastTextureWriter());
        }
        fillAdditionalOffscreenRenderDependencies(offscreenRenderDependencies);

        VectorUtil::removeDuplicates(offscreenRenderDependencies);
        return offscreenRenderDependencies;
    }

    void RenderTarget::fillAdditionalOffscreenRenderDependencies(std::vector<OffscreenRender*> &) const {
        //see override methods
    }

    void RenderTarget::configureWaitSemaphore(uint32_t frameIndex, VkSubmitInfo2& submitInfo, std::optional<VkSemaphoreSubmitInfo> additionalSemaphoreSubmitInfo) const {
        queueSubmitWaitSemaphoreSubmitInfos.clear();

        if (additionalSemaphoreSubmitInfo.has_value()) {
            queueSubmitWaitSemaphoreSubmitInfos.emplace_back(additionalSemaphoreSubmitInfo.value());
        }

        VkPipelineStageFlags2 stageMask = VK_PIPELINE_STAGE_2_NONE;
        if (hasGraphicsProcessors()) {
            stageMask |= VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
            if (hasDepthAttachment()) {
                stageMask |= VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT;
            }
        } else if (hasComputeProcessors()) {
            stageMask |= VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
        }

        for (OffscreenRender* offscreenRenderDependency : getOffscreenRenderDependencies()) {
            VkSemaphore waitSemaphore = offscreenRenderDependency->popSubmitSemaphore(frameIndex, name);
            if (waitSemaphore) {
                VkSemaphoreSubmitInfo semaphoreSubmitInfo{};
                semaphoreSubmitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
                semaphoreSubmitInfo.semaphore = waitSemaphore;
                semaphoreSubmitInfo.stageMask = stageMask; //wait at specified stages to receive the semaphore signal and continue the execution
                queueSubmitWaitSemaphoreSubmitInfos.emplace_back(semaphoreSubmitInfo);
            }
        }

        submitInfo.waitSemaphoreInfoCount = (uint32_t)queueSubmitWaitSemaphoreSubmitInfos.size();
        submitInfo.pWaitSemaphoreInfos = queueSubmitWaitSemaphoreSubmitInfos.empty() ? nullptr : queueSubmitWaitSemaphoreSubmitInfos.data();
    }

    void RenderTarget::updatePipelineProcessorData(uint32_t frameIndex) const {
        ScopeProfiler sp(Profiler::graphic(), "upShaderData");

        for (auto& processor : processors) {
            if (processor->isEnabled()) {
                processor->updatePipelineProcessorData(frameIndex);
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
            if (resultCmdBuffer != VK_SUCCESS) {
                throw std::runtime_error("Failed to begin recording command buffer with error code '" + std::string(string_VkResult(resultCmdBuffer)) + "' on render target: " + getName());
            }

            DebugLabelHelper::beginDebugRegion(commandBuffers[frameIndex], name, Vector4(0.9f, 1.0f, 0.8f, 1.0f));
            for (const TextureCopier& textureCopier : preRenderTextureCopier) {
                textureCopier.executeCopy(commandBuffers[frameIndex]);
            }
            for (std::size_t layerIndex = 0; layerIndex < framebuffers[frameIndex].size(); ++layerIndex) {
                if (hasGraphicsProcessors()) {
                    VkRenderPassBeginInfo renderPassInfo{};
                    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
                    renderPassInfo.renderPass = renderPass;
                    renderPassInfo.framebuffer = framebuffers[frameIndex][layerIndex];
                    renderPassInfo.renderArea.offset = {.x=0, .y=0};
                    renderPassInfo.renderArea.extent = {.width=getWidth(), .height=getHeight()};
                    renderPassInfo.clearValueCount = (uint32_t)clearValues.size();
                    renderPassInfo.pClearValues = clearValues.empty() ? nullptr : clearValues.data();

                    vkCmdBeginRenderPass(commandBuffers[frameIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
                }
                std::size_t boundPipelineId = 0;
                for (PipelineProcessor* pipelineProcessor: sortedEnabledProcessors) {
                    boundPipelineId = pipelineProcessor->updateCommandBuffer(commandBuffers[frameIndex], frameIndex, layerIndex, boundPipelineId);
                }
                if (hasGraphicsProcessors()) {
                    vkCmdEndRenderPass(commandBuffers[frameIndex]);
                }
            }
            framebufferDirty = false;
            DebugLabelHelper::endDebugRegion(commandBuffers[frameIndex]);

            VkResult resultEndCmdBuffer = vkEndCommandBuffer(commandBuffers[frameIndex]);
            if (resultEndCmdBuffer != VK_SUCCESS) {
                throw std::runtime_error("Failed to record command buffer with error code '" + std::string(string_VkResult(resultEndCmdBuffer)) + "' on render target: " + getName());
            }

            copiersDirty = false;
            processorsDirty = false;
        }
    }

}
