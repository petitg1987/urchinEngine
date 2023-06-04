#include <vulkan/vk_enum_string_helper.h>
#include <numeric>

#include <graphics/api/vulkan/render/GenericCompute.h>
#include <graphics/api/vulkan/setup/GraphicsSetupService.h>
#include <graphics/render/GenericComputeBuilder.h>
#include <graphics/texture/TextureReader.h>

namespace urchin {

    GenericCompute::GenericCompute(const GenericComputeBuilder& computeBuilder) :
            isInitialized(false),
            bIsEnabled(true),
            renderingOrder(0),
            name(computeBuilder.getName()),
            renderTarget(computeBuilder.getRenderTarget()),
            shader(computeBuilder.getShader()),
            uniformTextureReaders(computeBuilder.getUniformTextureReaders()),
            computeDescriptorSetLayout(nullptr),
            computePipelineLayout(nullptr),
            computePipeline(nullptr),
            descriptorPool(nullptr),
            drawCommandsDirty(false) {
        descriptorSetsDirty.resize(renderTarget.getNumFramebuffer(), false);

        if (renderTarget.isValidRenderTarget()) {
            for (const auto& uniformTextureReaderArray: uniformTextureReaders) {
                for (const auto& uniformTextureReader: uniformTextureReaderArray) {
                    uniformTextureReader->initialize();
                }
            }
        }

        initialize();
    }

    GenericCompute::~GenericCompute() {
        cleanup();
        uniformTextureReaders.clear();
        renderTarget.removeProcessor(this);
    }

    void GenericCompute::initialize() {
        ScopeProfiler sp(Profiler::graphic(), "genCompInit");
        assert(!isInitialized);

        if (renderTarget.isValidRenderTarget()) {
            createPipeline();
            createDescriptorPool();
            createDescriptorSets();
        }

        isInitialized = true;
    }

    void GenericCompute::cleanup() {
        if (isInitialized) {
            if (renderTarget.isValidRenderTarget()) {
                VkResult result = vkDeviceWaitIdle(GraphicsSetupService::instance().getDevices().getLogicalDevice());
                if (result != VK_SUCCESS) {
                    Logger::instance().logError("Failed to wait for device idle with error code '" + std::string(string_VkResult(result)) + "' on renderer: " + getName());
                } else {
                    destroyDescriptorSetsAndPool();
                    destroyPipeline();
                }
            }

            isInitialized = false;
        }
    }

    const std::string& GenericCompute::getName() const {
        return name;
    }

    const RenderTarget& GenericCompute::getRenderTarget() const {
        return renderTarget;
    }


    bool GenericCompute::needCommandBufferRefresh(std::size_t frameIndex) const {
        return drawCommandsDirty || descriptorSetsDirty[frameIndex];
    }

    bool GenericCompute::isEnabled() const {
        return bIsEnabled;
    }

    void GenericCompute::enableRenderer(unsigned int renderingOrder) {
        this->bIsEnabled = true;
        this->renderingOrder = renderingOrder;
        renderTarget.notifyProcessorEnabled(this);
    }

    void GenericCompute::disableRenderer() {
        bIsEnabled = false;
        renderTarget.notifyProcessorDisabled(this);
    }

    unsigned int GenericCompute::getRenderingOrder() const {
        return renderingOrder;
    }

    bool GenericCompute::isDepthTestEnabled() const {
        return false;
    }

    std::size_t GenericCompute::getPipelineId() const {
        return 123897456; //TODO pipeline->getId();
    }

    std::span<OffscreenRender*> GenericCompute::getTexturesWriter() const {
        return {}; //TODO impl...
    }

    void GenericCompute::createPipeline() {
        auto logicalDevice = GraphicsSetupService::instance().getDevices().getLogicalDevice();

        uint32_t shaderUniformBinding = 0;
        std::vector<VkDescriptorSetLayoutBinding> bindings;

        for (auto& uniformTextureReader : uniformTextureReaders) {
            VkDescriptorSetLayoutBinding samplerLayoutBinding{};
            samplerLayoutBinding.binding = shaderUniformBinding++;
            samplerLayoutBinding.descriptorCount = (uint32_t)uniformTextureReader.size();
            samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; //TODO or VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER ?
            samplerLayoutBinding.pImmutableSamplers = nullptr;
            samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
            bindings.emplace_back(samplerLayoutBinding);
        }

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = (uint32_t)bindings.size();
        layoutInfo.pBindings = bindings.empty() ? nullptr : bindings.data();

        VkResult result = vkCreateDescriptorSetLayout(logicalDevice, &layoutInfo, nullptr, &computeDescriptorSetLayout);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor set layout with error code: " + std::string(string_VkResult(result)));
        }

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &computeDescriptorSetLayout;

        VkResult pipelineLayoutResult = vkCreatePipelineLayout(logicalDevice, &pipelineLayoutInfo, nullptr, &computePipelineLayout);
        if (pipelineLayoutResult != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout with error code: " + std::string(string_VkResult(pipelineLayoutResult)));
        }

        VkComputePipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        pipelineInfo.layout = computePipelineLayout;
        pipelineInfo.stage = shader.getShaderStages()[0];

        VkResult pipelinesResult = vkCreateComputePipelines(logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &computePipeline);
        if (pipelinesResult != VK_SUCCESS) {
            throw std::runtime_error("Failed to create compute pipeline with error code: " + std::string(string_VkResult(pipelinesResult)));
        }
    }

    void GenericCompute::destroyPipeline() {
        auto logicalDevice = GraphicsSetupService::instance().getDevices().getLogicalDevice();

        vkDestroyDescriptorSetLayout(logicalDevice, computeDescriptorSetLayout, nullptr);
        vkDestroyPipeline(logicalDevice, computePipeline, nullptr);
        vkDestroyPipelineLayout(logicalDevice, computePipelineLayout, nullptr);
    }

    void GenericCompute::createDescriptorPool() {
        auto logicalDevice = GraphicsSetupService::instance().getDevices().getLogicalDevice();

        std::array<VkDescriptorPoolSize, 1> poolSizes{};

        int uniformTexReadersCount = 0;
        std::ranges::for_each(uniformTextureReaders, [&](auto& r){uniformTexReadersCount += (int)r.size();});
        int textureDescriptorCount = std::max(1, (int)renderTarget.getNumFramebuffer() * uniformTexReadersCount);
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[0].descriptorCount = (uint32_t)textureDescriptorCount;

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = (uint32_t)poolSizes.size();
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = (uint32_t)renderTarget.getNumFramebuffer();

        VkResult result = vkCreateDescriptorPool(logicalDevice, &poolInfo, nullptr, &descriptorPool);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor pool with error code '" + std::string(string_VkResult(result)) + "' on renderer: " + getName());
        }
    }

    void GenericCompute::createDescriptorSets() {
        auto logicalDevice = GraphicsSetupService::instance().getDevices().getLogicalDevice();

        std::vector<VkDescriptorSetLayout> layouts(renderTarget.getNumFramebuffer(), computeDescriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = (uint32_t)renderTarget.getNumFramebuffer();
        allocInfo.pSetLayouts = layouts.data();

        descriptorSets.resize(renderTarget.getNumFramebuffer());
        VkResult result = vkAllocateDescriptorSets(logicalDevice, &allocInfo, descriptorSets.data());
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate descriptor sets with error code '" + std::string(string_VkResult(result)) + "' on renderer: " + getName());
        }

        updateDescriptorSets();
    }


    void GenericCompute::updateDescriptorSets() {
        for (std::size_t frameIndex = 0; frameIndex < renderTarget.getNumFramebuffer(); frameIndex++) {
            updateDescriptorSets(frameIndex);
        }
    }

    void GenericCompute::updateDescriptorSets(std::size_t frameIndex) {
        descriptorWrites.clear();
        descriptorWrites.reserve(uniformTextureReaders.size());
        uint32_t shaderUniformBinding = 0;

        //textures
        imageInfosArray.clear();
        imageInfosArray.reserve(std::accumulate(uniformTextureReaders.begin(), uniformTextureReaders.end(), 0uL, [](std::size_t sum, const auto& utr) { return sum + utr.size(); }));
        for (const std::vector<std::shared_ptr<TextureReader>>& uniformTextureReaderArray : uniformTextureReaders) {
            std::size_t startIndex = imageInfosArray.size();

            for (const std::shared_ptr<TextureReader>& uniformTextureReader : uniformTextureReaderArray) {
                VkDescriptorImageInfo imageInfo{};
                imageInfo.imageLayout = uniformTextureReader->getTexture()->isDepthFormat() ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfo.imageView = uniformTextureReader->getTexture()->getImageView();
                imageInfo.sampler = uniformTextureReader->getParam().getTextureSampler();
                imageInfosArray.emplace_back(imageInfo);
            }

            VkWriteDescriptorSet textureDescriptorWrites{};
            textureDescriptorWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            textureDescriptorWrites.dstSet = descriptorSets[frameIndex];
            textureDescriptorWrites.dstBinding = shaderUniformBinding++;
            textureDescriptorWrites.dstArrayElement = 0;
            textureDescriptorWrites.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            textureDescriptorWrites.descriptorCount = (uint32_t)uniformTextureReaderArray.size();
            textureDescriptorWrites.pImageInfo = &imageInfosArray[startIndex]; //warning: imageInfosArray cannot be destroyed before calling vkUpdateDescriptorSets
            descriptorWrites.emplace_back(textureDescriptorWrites);
        }

        vkUpdateDescriptorSets(GraphicsSetupService::instance().getDevices().getLogicalDevice(), (uint32_t)(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

        drawCommandsDirty = true;
    }

    void GenericCompute::destroyDescriptorSetsAndPool() {
        vkDestroyDescriptorPool(GraphicsSetupService::instance().getDevices().getLogicalDevice(), descriptorPool, nullptr);
    }

    void GenericCompute::updateGraphicData(uint32_t) {

    }

    std::size_t GenericCompute::updateCommandBuffer(VkCommandBuffer commandBuffer, std::size_t frameIndex, std::size_t boundPipelineId) {
        ScopeProfiler sp(Profiler::graphic(), "upCmdBufComp");

        //TODO impl (priority)
        if (descriptorSetsDirty[frameIndex]) {
            updateDescriptorSets(frameIndex);
            descriptorSetsDirty[frameIndex] = false;
        }

        if (boundPipelineId != 123897456) { //TODO ...
            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline);
        }

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipelineLayout, 0, 1, &descriptorSets[frameIndex], 0, nullptr);

        vkCmdDispatch(commandBuffer, renderTarget.getWidth() / 16, renderTarget.getHeight() / 16, 1); //TODO 16 must match with code in compute shader

        drawCommandsDirty = false;
        return 123897456; //TODO ...
    }

}