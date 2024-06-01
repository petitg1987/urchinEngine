#include <libs/vkenum/vk_enum.h>
#include <numeric>

#include <graphics/api/vulkan/render/PipelineProcessor.h>
#include <graphics/api/vulkan/render/shader/Shader.h>
#include <graphics/api/vulkan/setup/GraphicsSetupService.h>
#include <graphics/texture/TextureReader.h>

namespace urchin {

    PipelineProcessor::PipelineProcessor(std::string name, RenderTarget& renderTarget, const Shader& shader,
                                         const std::map<uint32_t, ShaderDataContainer>& uniformData,
                                         const std::map<uint32_t, std::vector<std::shared_ptr<TextureReader>>>& uniformTextureReaders,
                                         const std::map<uint32_t, std::shared_ptr<Texture>>& uniformTextureOutputs) :
            name(std::move(name)),
            renderTarget(renderTarget),
            shader(shader),
            bIsEnabled(true),
            renderingOrder(0),
            uniformData(uniformData),
            uniformTextureReaders(uniformTextureReaders),
            uniformTextureOutputs(uniformTextureOutputs),
            descriptorPool(nullptr),
            drawCommandsDirty(false) {

        descriptorSetsDirty.resize(getRenderTarget().getNumFramebuffer(), false);

        if (getRenderTarget().isValidRenderTarget()) {
            for (const auto& [uniformBinding, uniformTextureReaderArray] : uniformTextureReaders) {
                for (const auto& uniformTextureReader : uniformTextureReaderArray) {
                    uniformTextureReader->initialize();
                }
            }
            for (const auto& [uniformBinding, uniformTextureOutput] : uniformTextureOutputs) {
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
        pipelineBuilder->setupUniform(uniformData, uniformTextureReaders, uniformTextureOutputs);
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
        for (auto& [uniformBinding, shaderDataContainer] : uniformData) {
            std::string bufferName = getName() + " - uniform" + std::to_string(uniformBinding);
            uniformsBuffers.insert({uniformBinding, AlterableBufferHandler()});
            uniformsBuffers.at(uniformBinding).initialize(bufferName, BufferHandler::UNIFORM, BufferHandler::DYNAMIC, getRenderTarget().getNumFramebuffer(), shaderDataContainer.getDataSize(), shaderDataContainer.getData());
            shaderDataContainer.markDataAsProcessed();
        }
    }

    void PipelineProcessor::destroyUniformBuffers() {
        for (auto& [uniformBinding, uniformsBuffer] : uniformsBuffers) {
            uniformsBuffer.cleanup();
        }
        uniformsBuffers.clear();
    }

    void PipelineProcessor::createDescriptorPool() {
        auto logicalDevice = GraphicsSetupService::instance().getDevices().getLogicalDevice();

        std::array<VkDescriptorPoolSize, 3> poolSizes{};

        int uboDescriptorCount = std::max(1, (int)getRenderTarget().getNumFramebuffer() * (int)uniformData.size());
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = (uint32_t)uboDescriptorCount;

        int uniformTexReadersCount = 0;
        std::ranges::for_each(uniformTextureReaders, [&](auto& r){uniformTexReadersCount += (int)r.second.size();});
        int textureDescriptorCount = std::max(1, (int)getRenderTarget().getNumFramebuffer() * uniformTexReadersCount);
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = (uint32_t)textureDescriptorCount;

        int uniformTexOutputsCount = std::max(1, (int)uniformTextureOutputs.size());
        poolSizes[2].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        poolSizes[2].descriptorCount = (uint32_t)uniformTexOutputsCount;

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

        std::vector<VkDescriptorSetLayout> layouts(getRenderTarget().getNumFramebuffer(), getPipeline().getDescriptorSetLayout());
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
        for (std::size_t frameIndex = 0; frameIndex < getRenderTarget().getNumFramebuffer(); frameIndex++) {
            updateDescriptorSets(frameIndex);
        }
    }

    void PipelineProcessor::updateDescriptorSets(std::size_t frameIndex) {
        descriptorWrites.clear();
        descriptorWrites.reserve(uniformData.size() + uniformTextureReaders.size() + uniformTextureOutputs.size());

        //uniform buffer objects
        bufferInfos.clear();
        bufferInfos.reserve(uniformData.size());
        for (const auto& [uniformBinding, uniformSingleData] : uniformData) {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = uniformsBuffers.at(uniformBinding).getBuffer(frameIndex);
            bufferInfo.offset = 0;
            bufferInfo.range = VK_WHOLE_SIZE;
            bufferInfos.emplace_back(bufferInfo);

            VkWriteDescriptorSet uniformDescriptorWrites{};
            uniformDescriptorWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            uniformDescriptorWrites.dstSet = descriptorSets[frameIndex];
            uniformDescriptorWrites.dstBinding = uniformBinding;
            uniformDescriptorWrites.dstArrayElement = 0;
            uniformDescriptorWrites.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            uniformDescriptorWrites.descriptorCount = 1;
            uniformDescriptorWrites.pBufferInfo = &bufferInfos.back(); //warning: bufferInfos cannot be destroyed before calling vkUpdateDescriptorSets
            descriptorWrites.emplace_back(uniformDescriptorWrites);
        }

        //textures
        imageInfosArray.clear();
        imageInfosArray.reserve(std::accumulate(uniformTextureReaders.begin(), uniformTextureReaders.end(), 0uL, [](std::size_t sum, const auto& utr) { return sum + utr.second.size(); }));
        for (const auto& [uniformBiding, uniformTextureReaderArray] : uniformTextureReaders) {
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
            textureDescriptorWrites.dstSet = descriptorSets[frameIndex];
            textureDescriptorWrites.dstBinding = uniformBiding;
            textureDescriptorWrites.dstArrayElement = 0;
            textureDescriptorWrites.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            textureDescriptorWrites.descriptorCount = (uint32_t)uniformTextureReaderArray.size();
            textureDescriptorWrites.pImageInfo = &imageInfosArray[startIndex]; //warning: imageInfosArray cannot be destroyed before calling vkUpdateDescriptorSets
            descriptorWrites.emplace_back(textureDescriptorWrites);
        }

        //texture outputs
        imageOutputInfosArray.clear();
        imageOutputInfosArray.reserve(uniformTextureOutputs.size());
        for (const auto& [uniformBinding, uniformTextureOutput] : uniformTextureOutputs) {
            std::size_t startIndex = imageOutputInfosArray.size();

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
            imageInfo.imageView = uniformTextureOutput->getImageView();
            imageInfo.sampler = VK_NULL_HANDLE;
            imageOutputInfosArray.emplace_back(imageInfo);

            VkWriteDescriptorSet textureDescriptorWrites{};
            textureDescriptorWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            textureDescriptorWrites.dstSet = descriptorSets[frameIndex];
            textureDescriptorWrites.dstBinding = uniformBinding;
            textureDescriptorWrites.dstArrayElement = 0;
            textureDescriptorWrites.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            textureDescriptorWrites.descriptorCount = (uint32_t)1;
            textureDescriptorWrites.pImageInfo = &imageOutputInfosArray[startIndex]; //warning: imageOutputInfosArray cannot be destroyed before calling vkUpdateDescriptorSets
            descriptorWrites.emplace_back(textureDescriptorWrites);
        }

        vkUpdateDescriptorSets(GraphicsSetupService::instance().getDevices().getLogicalDevice(), (uint32_t)(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

        drawCommandsDirty = true;
    }

    void PipelineProcessor::destroyDescriptorSetsAndPool() {
        vkDestroyDescriptorPool(GraphicsSetupService::instance().getDevices().getLogicalDevice(), descriptorPool, nullptr);
    }

    void PipelineProcessor::updateUniformData(uint32_t uniformBinding, const void* dataPtr) {
        #ifdef URCHIN_DEBUG
            assert(uniformData.contains(uniformBinding));
        #endif

        if (getRenderTarget().isValidRenderTarget()) {
            uniformData.at(uniformBinding).updateData(dataPtr);
        }
    }

    void PipelineProcessor::updateUniformTextureReader(uint32_t uniformBinding, const std::shared_ptr<TextureReader>& textureReader) {
        if (getRenderTarget().isValidRenderTarget()) {
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

        if (getRenderTarget().isValidRenderTarget()) {
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

        for (auto& [uniformBinding, uniformTextureReaderArray] : uniformTextureReaders) {
            for (auto& uniformTextureReader : uniformTextureReaderArray) {
                auto* lastTextureWriter = uniformTextureReader->getTexture()->getLastTextureWriter();
                if (lastTextureWriter) {
                    texturesWriter.emplace_back(lastTextureWriter);
                }
            }
        }

        return texturesWriter;
    }

    void PipelineProcessor::updatePipelineProcessorData(uint32_t frameIndex) {
        //update shader uniforms
        for (auto& [uniformBinding, dataContainer] : uniformData) {
            if (dataContainer.hasNewData(frameIndex)) {
                if (uniformsBuffers.at(uniformBinding).updateData(frameIndex, dataContainer.getDataSize(), dataContainer.getData())) {
                    markDrawCommandsDirty();
                }
                dataContainer.markDataAsProcessed(frameIndex);
            }
        }
    }

    void PipelineProcessor::markDrawCommandsDirty() {
        drawCommandsDirty = true;
    }

    std::vector<VkDescriptorSet>& PipelineProcessor::getDescriptorSets() {
        return descriptorSets;
    }

    bool PipelineProcessor::needCommandBufferRefresh(std::size_t) const { //TODO mark dirty if several output layer !
        return true; //drawCommandsDirty || descriptorSetsDirty[frameIndex];
    }

    std::size_t PipelineProcessor::updateCommandBuffer(VkCommandBuffer commandBuffer, std::size_t frameIndex, std::size_t layerIndex, std::size_t boundPipelineId) {
        if (descriptorSetsDirty[frameIndex]) {
            updateDescriptorSets(frameIndex);
            descriptorSetsDirty[frameIndex] = false;
        }

        doUpdateCommandBuffer(commandBuffer, frameIndex, layerIndex, boundPipelineId);

        drawCommandsDirty = false;
        return getPipeline().getId();
    }

}