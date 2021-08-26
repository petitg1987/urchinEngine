#include <algorithm>
#include <cassert>

#include <graphic/render/target/RenderTarget.h>
#include <graphic/setup/GraphicService.h>
#include <graphic/helper/DebugLabelHelper.h>
#include <graphic/helper/ImageHelper.h>
#include <graphic/render/GenericRenderer.h>
#include <graphic/render/target/OffscreenRender.h>

namespace urchin {

    RenderTarget::RenderTarget(std::string name, DepthAttachmentType depthAttachmentType) :
            isInitialized(false),
            name(std::move(name)),
            depthAttachmentType(depthAttachmentType),
            renderPass(nullptr),
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
            throw std::runtime_error("Can not define an external depth texture. Wrong type of depth attachment: " + std::to_string(depthAttachmentType));
        }
        this->externalDepthTexture = externalDepthTexture;
    }

    const std::shared_ptr<Texture>& RenderTarget::getDepthTexture() const {
        if (depthAttachmentType == NO_DEPTH_ATTACHMENT) {
            throw std::runtime_error("Cannot retrieve depth texture on a render target created without a depth attachment");
        } else if (depthAttachmentType == LOCAL_DEPTH_ATTACHMENT) {
            throw std::runtime_error("Cannot retrieve depth texture on a render target created with a local depth attachment");
        } else if (depthAttachmentType == SHARED_DEPTH_ATTACHMENT) {
            return depthTexture;
        } else if (depthAttachmentType == EXTERNAL_DEPTH_ATTACHMENT) {
            return externalDepthTexture;
        }
        throw std::runtime_error("Unknown depth attachment type: " + std::to_string(depthAttachmentType));
    }

    void RenderTarget::addRenderer(GenericRenderer* renderer) {
        #ifndef NDEBUG
            assert(&renderer->getRenderTarget() == this);
            for (auto* r : renderers) {
                assert(r != renderer);
            }
        #endif

        renderers.emplace_back(renderer);
        renderersDirty = true;
    }

    void RenderTarget::removeRenderer(const GenericRenderer* renderer) {
        renderers.erase(std::remove(renderers.begin(), renderers.end(), renderer), renderers.end());
        renderersDirty = true;
    }

    void RenderTarget::notifyRendererEnabled(GenericRenderer* genericRenderer) {
        if (!genericRenderer->isEnabled()) {
            assert(false);
        }

        //move generic renderer at the end of the renderers list
        for (auto it = renderers.begin(); it != renderers.end(); ++it) {
            if (*it == genericRenderer) {
                renderers.push_back(*it);
                renderers.erase(it);
                break;
            }
        }

        renderersDirty = true;
    }

    void RenderTarget::notifyRendererDisabled(GenericRenderer* genericRenderer) {
        if (genericRenderer->isEnabled()) {
            assert(false);
        }

        renderersDirty = true;
    }

    void RenderTarget::disableAllRenderers() {
        for (auto& renderer: renderers) {
            if (renderer->isEnabled()) {
                renderer->disableRenderer();
            }
        }
    }

    bool RenderTarget::needCommandBuffersRefresh() {
        const auto& rendererHasDrawCmdDirty = [](const auto* renderer){
            return renderer->isEnabled() && renderer->isDrawCommandDirty();
        };
        return renderersDirty || std::any_of(renderers.begin(), renderers.end(), rendererHasDrawCmdDirty);
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

    bool RenderTarget::hasRenderer() {
        const auto& rendererIsEnabled = [](const auto* renderer){return renderer->isEnabled();};
        return std::any_of(renderers.begin(), renderers.end(), rendererIsEnabled);
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
            throw std::runtime_error("Unknown depth attachment type: " + std::to_string(depthAttachmentType));
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

        VkResult result = vkCreateRenderPass(GraphicService::instance().getDevices().getLogicalDevice(), &renderPassInfo, nullptr, &renderPass);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create render pass with error code: " + std::to_string(result));
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
                    depthTexture = Texture::build(getWidth(), getHeight(), TextureFormat::DEPTH_32_FLOAT, nullptr);
                } else {
                    depthTexture = Texture::buildArray(getWidth(), getHeight(), getLayer(), TextureFormat::DEPTH_32_FLOAT, nullptr);
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

    void RenderTarget::addNewFrameBuffer(const std::vector<VkImageView>& attachments) {
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
            throw std::runtime_error("Failed to create framebuffer with error code: " + std::to_string(result));
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
            throw std::runtime_error("Failed to allocate command buffers with error code: " + std::to_string(resultCommandBuffers));
        }
    }

    void RenderTarget::createCommandPool() {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = GraphicService::instance().getQueues().getGraphicsQueueFamily();
        poolInfo.flags = 0;

        VkResult result = vkCreateCommandPool(GraphicService::instance().getDevices().getLogicalDevice(), &poolInfo, nullptr, &commandPool);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create command pool with error code: " + std::to_string(result));
        }
    }

    void RenderTarget::destroyCommandBuffersAndPool() {
        vkDestroyCommandPool(GraphicService::instance().getDevices().getLogicalDevice(), commandPool, nullptr);
    }

    const std::vector<OffscreenRender*>& RenderTarget::getRenderDependencies() const {
        renderDependencies.clear();
        for (auto& renderer : renderers) {
            if (renderer->isEnabled()) {
                const auto& renderTextureWriter = renderer->getTexturesWriter();
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

    void RenderTarget::configureWaitSemaphore(VkSubmitInfo& submitInfo, VkSemaphore additionalCustomSemaphore) const {
        const std::vector<OffscreenRender*>& renderDependencies = getRenderDependencies();

        queueSubmitWaitSemaphores.clear();
        queueSubmitWaitStages.clear();

        if (additionalCustomSemaphore) {
            queueSubmitWaitSemaphores.emplace_back(additionalCustomSemaphore);
            queueSubmitWaitStages.emplace_back(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
        }

        for (auto& renderDependency : renderDependencies) {
            VkSemaphore queueSubmitSemaphore = renderDependency->retrieveQueueSubmitSemaphoreAndFlagUsed();
            if (queueSubmitSemaphore) {
                queueSubmitWaitSemaphores.emplace_back(queueSubmitSemaphore);
                queueSubmitWaitStages.emplace_back(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
            }
        }

        submitInfo.waitSemaphoreCount = (uint32_t)queueSubmitWaitSemaphores.size();
        submitInfo.pWaitSemaphores = queueSubmitWaitSemaphores.data();
        submitInfo.pWaitDstStageMask = queueSubmitWaitStages.data();
    }

    void RenderTarget::updateGraphicData(uint32_t frameIndex) {
        ScopeProfiler sp(Profiler::graphic(), "upShaderData");

        for (auto& renderer : renderers) {
            if (renderer->isEnabled()) {
                renderer->updateGraphicData(frameIndex);
            }
        }
    }

    void RenderTarget::updateCommandBuffers(const std::vector<VkClearValue>& clearValues) {
        ScopeProfiler sp(Profiler::graphic(), "upCmdBuffers");
        if (!needCommandBuffersRefresh()) {
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
        vkResetCommandPool(GraphicService::instance().getDevices().getLogicalDevice(), commandPool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);

        for (std::size_t i = 0; i < commandBuffers.size(); i++) {
            VkResult resultCommandBuffer = vkBeginCommandBuffer(commandBuffers[i], &beginInfo);
            if (resultCommandBuffer != VK_SUCCESS) {
                throw std::runtime_error("Failed to begin recording command buffer with error code: " + std::to_string(resultCommandBuffer));
            }

            renderPassInfo.framebuffer = framebuffers[i];

            DebugLabelHelper::beginDebugRegion(commandBuffers[i], name, Vector4<float>(0.9f, 1.0f, 0.8f, 1.0f));
            vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
            for (auto& renderer : renderers) {
                if (renderer->isEnabled()) {
                    renderer->updateCommandBuffer(commandBuffers[i], i);
                }
            }
            vkCmdEndRenderPass(commandBuffers[i]);
            DebugLabelHelper::endDebugRegion(commandBuffers[i]);

            VkResult resultEndCommandBuffer = vkEndCommandBuffer(commandBuffers[i]);
            if (resultEndCommandBuffer != VK_SUCCESS) {
                throw std::runtime_error("Failed to record command buffer with error code: " + std::to_string(resultEndCommandBuffer));
            }
        }

        renderersDirty = false;
    }

}
