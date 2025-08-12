#include <numeric>
#include <ranges>

#include "libs/vkenum/vk_enum.h"
#include "graphics/api/vulkan/render/PipelineProcessor.h"
#include "graphics/api/vulkan/render/shader/Shader.h"
#include "graphics/api/vulkan/setup/GraphicsSetupService.h"
#include "graphics/texture/TextureReader.h"

namespace urchin {

    PipelineProcessor::PipelineProcessor(std::string name, RenderTarget& renderTarget, const Shader& shader,
                                         const std::map<uint32_t, ShaderDataContainer>& uniformData,
                                         const std::map<uint32_t, std::vector<std::shared_ptr<TextureReader>>>& uniformTextureReaders,
                                         const std::map<uint32_t, std::shared_ptr<Texture>>& uniformTextureOutputs,
                                         const std::map<uint32_t, ShaderDataContainer>& storageBufferData) :
            name(std::move(name)),
            renderTarget(renderTarget),
            shader(shader),
            bIsEnabled(true),
            renderingOrder(0),
            uniformData(uniformData),
            uniformTextureReaders(uniformTextureReaders),
            uniformTextureOutputs(uniformTextureOutputs),
            storageBufferData(storageBufferData),
            descriptorPool(nullptr),
            drawCommandsDirty(false) {

        descriptorSetsDirty.resize(getRenderTarget().getNumFramebuffer(), false);

        if (!getRenderTarget().isTestMode()) {
            for (const std::vector<std::shared_ptr<TextureReader>>& uniformTextureReaderArray : std::views::values(uniformTextureReaders)) {
                for (const auto& uniformTextureReader : uniformTextureReaderArray) {
                    uniformTextureReader->initialize();
                }
            }
            for (const std::shared_ptr<Texture>& uniformTextureOutput : std::views::values(uniformTextureOutputs)) {
                uniformTextureOutput->initialize();
            }
        }
    }

    PipelineProcessor::~PipelineProcessor() {
        uniformTextureReaders.clear();
        uniformTextureOutputs.clear();
        renderTarget.removeProcessor(this);
    }

    const std::string& PipelineProcessor::getName() const {
        return name;
    }

    const RenderTarget& PipelineProcessor::getRenderTarget() const {
        return renderTarget;
    }

    const Shader& PipelineProcessor::getShader() const {
        return shader;
    }

    bool PipelineProcessor::isEnabled() const {
        return bIsEnabled;
    }

    void PipelineProcessor::enableRenderer(unsigned int renderingOrder) {
        this->bIsEnabled = true;
        this->renderingOrder = renderingOrder;
        renderTarget.notifyProcessorEnabled(this);
    }

    void PipelineProcessor::disableRenderer() {
        bIsEnabled = false;
        renderTarget.notifyProcessorDisabled(this);
    }

    unsigned int PipelineProcessor::getRenderingOrder() const {
        return renderingOrder;
    }

    void PipelineProcessor::setupPipelineBuilder(std::unique_ptr<PipelineBuilder> pipelineBuilder) {
        assert(!this->pipelineBuilder);
        this->pipelineBuilder = std::move(pipelineBuilder);
    }

    PipelineBuilder& PipelineProcessor::getPipelineBuilder() const {
        assert(this->pipelineBuilder);
        return *pipelineBuilder;
    }

    void PipelineProcessor::createPipeline() {
        assert(pipelineBuilder);
        pipelineBuilder->setupUniform(uniformData, uniformTextureReaders, uniformTextureOutputs, storageBufferData);
        pipeline = pipelineBuilder->buildPipeline();
    }

    void PipelineProcessor::destroyPipeline() {
        pipeline = nullptr;
    }

    Pipeline& PipelineProcessor::getPipeline() const {
        return *pipeline;
    }

    std::size_t PipelineProcessor::getPipelineId() const {
        assert(pipeline);
        return pipeline->getId();
    }

    PipelineType PipelineProcessor::getPipelineType() const {
        assert(pipeline);
        return pipeline->getType();
    }

    void PipelineProcessor::createUniformBuffers() {
        for (auto& [binding, shaderDataContainer] : uniformData) {
            std::string bufferName = getName() + " - uniform" + std::to_string(binding);
            uniformsBuffers.insert({binding, AlterableBufferHandler()});
            uniformsBuffers.at(binding).initialize(bufferName, BufferHandler::UNIFORM, BufferHandler::DYNAMIC, getRenderTarget().getNumFramebuffer(), shaderDataContainer.getDataSize(), shaderDataContainer.getData());
            shaderDataContainer.markDataAsProcessed();
        }
    }

    void PipelineProcessor::destroyUniformBuffers() {
        for (AlterableBufferHandler& uniformsBuffer : std::views::values(uniformsBuffers)) {
            uniformsBuffer.cleanup();
        }
        uniformsBuffers.clear();
    }

    void PipelineProcessor::createStorageBuffers() {
        for (auto& [binding, shaderDataContainer] : storageBufferData) {
            std::string bufferName = getName() + " - storageBuffer" + std::to_string(binding);
            storageBuffers.insert({binding, AlterableBufferHandler()}); //TODO review type of buffer ?
            storageBuffers.at(binding).initialize(bufferName, BufferHandler::UNIFORM, BufferHandler::DYNAMIC, getRenderTarget().getNumFramebuffer(), shaderDataContainer.getDataSize(), shaderDataContainer.getData());
            shaderDataContainer.markDataAsProcessed();
        }
    }

    void PipelineProcessor::destroyStorageBuffers() {
        for (AlterableBufferHandler& storageBuffer : std::views::values(storageBuffers)) {
            storageBuffer.cleanup();
        }
        storageBuffers.clear();
    }

    void PipelineProcessor::createDescriptorPool() {
        auto logicalDevice = GraphicsSetupService::instance().getDevices().getLogicalDevice();

        std::array<VkDescriptorPoolSize, 4> poolSizes{};

        int uniformCount = std::max(1, (int)getRenderTarget().getNumFramebuffer() * (int)uniformData.size());
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = (uint32_t)uniformCount;

        int uniformTexReadersCount = 0;
        std::ranges::for_each(uniformTextureReaders, [&](auto& r){uniformTexReadersCount += (int)r.second.size();});
        int textureDescriptorCount = std::max(1, (int)getRenderTarget().getNumFramebuffer() * uniformTexReadersCount);
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = (uint32_t)textureDescriptorCount;

        int uniformTexOutputsCount = std::max(1, (int)uniformTextureOutputs.size());
        poolSizes[2].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        poolSizes[2].descriptorCount = (uint32_t)uniformTexOutputsCount;

        int storageBufferCount = std::max(1, (int)getRenderTarget().getNumFramebuffer() * (int)storageBufferData.size());
        poolSizes[3].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        poolSizes[3].descriptorCount = (uint32_t)storageBufferCount;

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = (uint32_t)poolSizes.size();
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = (uint32_t)getRenderTarget().getNumFramebuffer();

        VkResult result = vkCreateDescriptorPool(logicalDevice, &poolInfo, nullptr, &descriptorPool);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor pool with error code '" + std::string(string_VkResult(result)) + "' on renderer: " + getName());
        }
    }

    void PipelineProcessor::createDescriptorSets() {
        auto logicalDevice = GraphicsSetupService::instance().getDevices().getLogicalDevice();

        std::vector layouts(getRenderTarget().getNumFramebuffer(), getPipeline().getDescriptorSetLayout());
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = (uint32_t)getRenderTarget().getNumFramebuffer();
        allocInfo.pSetLayouts = layouts.data();

        descriptorSets.resize(getRenderTarget().getNumFramebuffer());
        VkResult result = vkAllocateDescriptorSets(logicalDevice, &allocInfo, descriptorSets.data());
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate descriptor sets with error code '" + std::string(string_VkResult(result)) + "' on renderer: " + getName());
        }

        updateDescriptorSets();
    }

    void PipelineProcessor::updateDescriptorSets() {
        for (std::size_t framebufferIndex = 0; framebufferIndex < getRenderTarget().getNumFramebuffer(); framebufferIndex++) {
            updateDescriptorSets(framebufferIndex);
        }
    }

    void PipelineProcessor::updateDescriptorSets(std::size_t framebufferIndex) {
        descriptorWrites.clear();
        descriptorWrites.reserve(uniformData.size() + uniformTextureReaders.size() + uniformTextureOutputs.size() + storageBufferData.size());

        //uniform buffer objects
        uniformBufferInfos.clear();
        uniformBufferInfos.reserve(uniformData.size());
        for (uint32_t binding : std::views::keys(uniformData)) {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = uniformsBuffers.at(binding).getBuffer(framebufferIndex);
            bufferInfo.offset = 0;
            bufferInfo.range = VK_WHOLE_SIZE;
            uniformBufferInfos.emplace_back(bufferInfo);

            VkWriteDescriptorSet uniformDescriptorWrites{};
            uniformDescriptorWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            uniformDescriptorWrites.dstSet = descriptorSets[framebufferIndex];
            uniformDescriptorWrites.dstBinding = binding;
            uniformDescriptorWrites.dstArrayElement = 0;
            uniformDescriptorWrites.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            uniformDescriptorWrites.descriptorCount = 1;
            uniformDescriptorWrites.pBufferInfo = &uniformBufferInfos.back(); //warning: uniformBufferInfos cannot be destroyed before calling vkUpdateDescriptorSets
            descriptorWrites.emplace_back(uniformDescriptorWrites);
        }

        //uniform texture readers
        imageInfosArray.clear();
        imageInfosArray.reserve(std::accumulate(uniformTextureReaders.begin(), uniformTextureReaders.end(), 0uL, [](std::size_t sum, const auto& utr) { return sum + utr.second.size(); }));
        for (const auto& [binding, uniformTextureReaderArray] : uniformTextureReaders) {
            std::size_t startIndex = imageInfosArray.size();

            for (const std::shared_ptr<TextureReader>& uniformTextureReader : uniformTextureReaderArray) {
                VkDescriptorImageInfo imageInfo{};
                VkImageLayout imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                if (uniformTextureReader->getTexture()->isDepthFormat()) {
                    imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
                } else if (uniformTextureReader->getTexture()->getOutputUsage() == OutputUsage::COMPUTE) {
                    imageLayout = VK_IMAGE_LAYOUT_GENERAL;
                }
                imageInfo.imageLayout = imageLayout;
                imageInfo.imageView = uniformTextureReader->getTexture()->getImageView();
                imageInfo.sampler = uniformTextureReader->getParam().getTextureSampler();
                imageInfosArray.emplace_back(imageInfo);
            }

            VkWriteDescriptorSet textureDescriptorWrites{};
            textureDescriptorWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            textureDescriptorWrites.dstSet = descriptorSets[framebufferIndex];
            textureDescriptorWrites.dstBinding = binding;
            textureDescriptorWrites.dstArrayElement = 0;
            textureDescriptorWrites.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            textureDescriptorWrites.descriptorCount = (uint32_t)uniformTextureReaderArray.size();
            textureDescriptorWrites.pImageInfo = &imageInfosArray[startIndex]; //warning: imageInfosArray cannot be destroyed before calling vkUpdateDescriptorSets
            descriptorWrites.emplace_back(textureDescriptorWrites);
        }

        //uniform texture outputs
        imageOutputInfosArray.clear();
        imageOutputInfosArray.reserve(uniformTextureOutputs.size());
        for (const auto& [binding, uniformTextureOutput] : uniformTextureOutputs) {
            std::size_t startIndex = imageOutputInfosArray.size();

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
            imageInfo.imageView = uniformTextureOutput->getImageView();
            imageInfo.sampler = VK_NULL_HANDLE;
            imageOutputInfosArray.emplace_back(imageInfo);

            VkWriteDescriptorSet textureDescriptorWrites{};
            textureDescriptorWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            textureDescriptorWrites.dstSet = descriptorSets[framebufferIndex];
            textureDescriptorWrites.dstBinding = binding;
            textureDescriptorWrites.dstArrayElement = 0;
            textureDescriptorWrites.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            textureDescriptorWrites.descriptorCount = (uint32_t)1;
            textureDescriptorWrites.pImageInfo = &imageOutputInfosArray[startIndex]; //warning: imageOutputInfosArray cannot be destroyed before calling vkUpdateDescriptorSets
            descriptorWrites.emplace_back(textureDescriptorWrites);
        }

        //storage buffer objects
        storageBufferInfos.clear();
        storageBufferInfos.reserve(storageBufferData.size());
        for (uint32_t binding : std::views::keys(storageBufferData)) {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = storageBuffers.at(binding).getBuffer(framebufferIndex);
            bufferInfo.offset = 0;
            bufferInfo.range = VK_WHOLE_SIZE;
            storageBufferInfos.emplace_back(bufferInfo);

            VkWriteDescriptorSet uniformDescriptorWrites{};
            uniformDescriptorWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            uniformDescriptorWrites.dstSet = descriptorSets[framebufferIndex];
            uniformDescriptorWrites.dstBinding = binding;
            uniformDescriptorWrites.dstArrayElement = 0;
            uniformDescriptorWrites.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            uniformDescriptorWrites.descriptorCount = 1;
            uniformDescriptorWrites.pBufferInfo = &storageBufferInfos.back(); //warning: storageBufferInfos cannot be destroyed before calling vkUpdateDescriptorSets
            descriptorWrites.emplace_back(uniformDescriptorWrites);
        }

        vkUpdateDescriptorSets(GraphicsSetupService::instance().getDevices().getLogicalDevice(), (uint32_t)(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

        drawCommandsDirty = true;
    }

    void PipelineProcessor::destroyDescriptorSetsAndPool() const {
        vkDestroyDescriptorPool(GraphicsSetupService::instance().getDevices().getLogicalDevice(), descriptorPool, nullptr);
    }

    void PipelineProcessor::updateUniformData(uint32_t uniformBinding, const void* dataPtr) {
        #ifdef URCHIN_DEBUG
            assert(uniformData.contains(uniformBinding));
        #endif

        if (!getRenderTarget().isTestMode()) {
            uniformData.at(uniformBinding).updateData(dataPtr);
        }
    }

    void PipelineProcessor::updateUniformTextureReader(uint32_t uniformBinding, const std::shared_ptr<TextureReader>& textureReader) {
        if (!getRenderTarget().isTestMode()) {
            updateUniformTextureReaderArray(uniformBinding, 0, textureReader);
        }
    }

    const std::shared_ptr<TextureReader>& PipelineProcessor::getUniformTextureReader(uint32_t uniformBinding) const {
        #ifdef URCHIN_DEBUG
            assert(uniformTextureReaders.contains(uniformBinding));
            assert(uniformTextureReaders.at(uniformBinding).size() == 1);
        #endif
        return getUniformTextureReaderArray(uniformBinding)[0];
    }

    const std::shared_ptr<TextureReader>& PipelineProcessor::getUniformTextureReader(uint32_t uniformBinding, std::size_t textureIndex) const {
        #ifdef URCHIN_DEBUG
            assert(uniformTextureReaders.contains(uniformBinding));
            assert(uniformTextureReaders.at(uniformBinding).size() > textureIndex);
        #endif
        return getUniformTextureReaderArray(uniformBinding)[textureIndex];
    }

    void PipelineProcessor::updateUniformTextureReaderArray(uint32_t uniformBinding, std::size_t textureIndex, const std::shared_ptr<TextureReader>& textureReader) {
        #ifdef URCHIN_DEBUG
            assert(uniformTextureReaders.contains(uniformBinding));
            assert(uniformTextureReaders.at(uniformBinding).size() > textureIndex);
        #endif

        if (!getRenderTarget().isTestMode()) {
            textureReader->initialize();
            uniformTextureReaders.at(uniformBinding)[textureIndex] = textureReader;

            std::fill(descriptorSetsDirty.begin(), descriptorSetsDirty.end(), true);
        }
    }

    const std::vector<std::shared_ptr<TextureReader>>& PipelineProcessor::getUniformTextureReaderArray(uint32_t uniformBinding) const {
        #ifdef URCHIN_DEBUG
            assert(uniformTextureReaders.contains(uniformBinding));
        #endif
        return uniformTextureReaders.at(uniformBinding);
    }

    std::span<OffscreenRender*> PipelineProcessor::getTexturesWriter() const {
        texturesWriter.clear();

        for (const std::vector<std::shared_ptr<TextureReader>>& uniformTextureReaderArray : std::views::values(uniformTextureReaders)) {
            for (const std::shared_ptr<TextureReader>& uniformTextureReader : uniformTextureReaderArray) {
                OffscreenRender* lastTextureWriter = uniformTextureReader->getTexture()->getLastTextureWriter();
                if (lastTextureWriter) {
                    texturesWriter.emplace_back(lastTextureWriter);
                }
            }
        }

        return texturesWriter;
    }

    void PipelineProcessor::updatePipelineProcessorData(uint32_t framebufferIndex) {
        //update shader uniforms
        for (auto& [binding, dataContainer] : uniformData) {
            if (dataContainer.hasNewData(framebufferIndex)) {
                if (uniformsBuffers.at(binding).updateData(framebufferIndex, dataContainer.getDataSize(), dataContainer.getData())) {
                    markDrawCommandsDirty();
                }
                dataContainer.markDataAsProcessed(framebufferIndex);
            }
        }

        //update shader storage
        for (auto& [binding, dataContainer] : storageBufferData) {
            if (dataContainer.hasNewData(framebufferIndex)) {
                if (storageBuffers.at(binding).updateData(framebufferIndex, dataContainer.getDataSize(), dataContainer.getData())) {
                    markDrawCommandsDirty();
                }
                dataContainer.markDataAsProcessed(framebufferIndex);
            }
        }
    }

    void PipelineProcessor::markDrawCommandsDirty() {
        drawCommandsDirty = true;
    }

    std::vector<VkDescriptorSet>& PipelineProcessor::getDescriptorSets() {
        return descriptorSets;
    }

    bool PipelineProcessor::needCommandBufferRefresh(std::size_t framebufferIndex) const {
        return drawCommandsDirty || descriptorSetsDirty[framebufferIndex];
    }

    std::size_t PipelineProcessor::updateCommandBuffer(VkCommandBuffer commandBuffer, std::size_t framebufferIndex, std::size_t layerIndex, std::size_t currentBoundPipelineId) {
        if (isApplicableOnLayer(layerIndex)) {
            if (descriptorSetsDirty[framebufferIndex]) {
                updateDescriptorSets(framebufferIndex);
                descriptorSetsDirty[framebufferIndex] = false;
            }

            std::size_t newBoundPipelineId = doUpdateCommandBuffer(commandBuffer, framebufferIndex, layerIndex, currentBoundPipelineId);

            drawCommandsDirty = false;
            return newBoundPipelineId;
        }
        return currentBoundPipelineId;
    }

}