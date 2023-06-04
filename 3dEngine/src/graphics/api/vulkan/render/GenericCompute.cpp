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
            uniformData(computeBuilder.getUniformData()),
            uniformTextureReaders(computeBuilder.getUniformTextureReaders()),
            descriptorPool(nullptr),
            drawCommandsDirty(false) {
        descriptorSetsDirty.resize(renderTarget.getNumFramebuffer(), false);

        pipelineBuilder = std::make_unique<PipelineBuilder>(PipelineType::COMPUTE, name);
        pipelineBuilder->setupRenderTarget(renderTarget);
        pipelineBuilder->setupShader(shader);

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
            createUniformBuffers();
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
                    destroyUniformBuffers();
                    destroyPipeline();
                }
            }

            isInitialized = false;
        }
    }

    bool GenericCompute::isCompute() const {
        return true;
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
        return pipeline->getId();
    }

    std::span<OffscreenRender*> GenericCompute::getTexturesWriter() const {
        texturesWriter.clear();

        for (auto& uniformTextureReaderArray : uniformTextureReaders) {
            for (auto& uniformTextureReader : uniformTextureReaderArray) {
                auto lastTextureWriter = uniformTextureReader->getTexture()->getLastTextureWriter();
                if (lastTextureWriter) {
                    texturesWriter.emplace_back(lastTextureWriter);
                }
            }
        }

        return texturesWriter;
    }

    void GenericCompute::createPipeline() {
        pipelineBuilder->setupUniform(uniformData, uniformTextureReaders);
        pipeline = pipelineBuilder->buildPipeline();
    }

    void GenericCompute::destroyPipeline() {
        pipeline = nullptr;
    }

    void GenericCompute::createUniformBuffers() {
        uniformsBuffers.resize(uniformData.size());
        for (std::size_t dataIndex = 0; dataIndex < uniformData.size(); ++dataIndex) {
            ShaderDataContainer& shaderDataContainer = uniformData[dataIndex];
            std::string bufferName = getName() + " - uniform" + std::to_string(dataIndex);
            uniformsBuffers[dataIndex].initialize(bufferName, BufferHandler::UNIFORM, BufferHandler::DYNAMIC, renderTarget.getNumFramebuffer(), shaderDataContainer.getDataSize(), shaderDataContainer.getData());
            shaderDataContainer.markDataAsProcessed();
        }
    }

    void GenericCompute::destroyUniformBuffers() {
        for (auto& uniformsBuffer : uniformsBuffers) {
            uniformsBuffer.cleanup();
        }
        uniformsBuffers.clear();
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

        std::vector<VkDescriptorSetLayout> layouts(renderTarget.getNumFramebuffer(), pipeline->getDescriptorSetLayout());
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

    void GenericCompute::updateGraphicData(uint32_t frameIndex) {
        //update shader uniforms
        for (std::size_t uniformDataIndex = 0; uniformDataIndex < uniformData.size(); ++uniformDataIndex) {
            if (uniformData[uniformDataIndex].hasNewData(frameIndex)) {
                auto& dataContainer = uniformData[uniformDataIndex];
                drawCommandsDirty |= uniformsBuffers[uniformDataIndex].updateData(frameIndex, dataContainer.getDataSize(), dataContainer.getData());
                dataContainer.markDataAsProcessed(frameIndex);
            }
        }
    }

    std::size_t GenericCompute::updateCommandBuffer(VkCommandBuffer commandBuffer, std::size_t frameIndex, std::size_t boundPipelineId) {
        ScopeProfiler sp(Profiler::graphic(), "upCmdBufComp");

        if (descriptorSetsDirty[frameIndex]) {
            updateDescriptorSets(frameIndex);
            descriptorSetsDirty[frameIndex] = false;
        }

        if (boundPipelineId != pipeline->getId()) {
            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->getPipeline());
        }

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->getPipelineLayout(), 0, 1, &descriptorSets[frameIndex], 0, nullptr);

        vkCmdDispatch(commandBuffer, renderTarget.getWidth() / 16, renderTarget.getHeight() / 16, 1); //TODO 16 must match with code in compute shader

        drawCommandsDirty = false;
        return pipeline->getId();
    }

}