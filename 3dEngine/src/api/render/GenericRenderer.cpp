#include <libs/vma/vk_mem_alloc.h>

#include <api/render/GenericRenderer.h>
#include <api/helper/DebugLabelHelper.h>
#include <api/setup/GraphicService.h>
#include <api/render/GenericRendererBuilder.h>
#include <api/render/model/DataContainer.h>
#include <api/render/pipeline/PipelineBuilder.h>

namespace urchin {

    GenericRenderer::GenericRenderer(const GenericRendererBuilder& rendererBuilder) :
            isInitialized(false),
            bIsEnabled(true),
            renderingOrder(0),
            name(rendererBuilder.getName()),
            renderTarget(rendererBuilder.getRenderTarget()),
            shader(rendererBuilder.getShader()),
            data(rendererBuilder.getData()),
            instanceData(rendererBuilder.getInstanceData()),
            indices(rendererBuilder.getIndices()),
            uniformData(rendererBuilder.getUniformData()),
            uniformTextureReaders(rendererBuilder.getUniformTextureReaders()),
            depthTestEnabled(rendererBuilder.isDepthTestEnabled()),
            descriptorPool(nullptr),
            drawCommandDirty(false) {
        pipelineBuilder = std::make_unique<PipelineBuilder>(name);
        pipelineBuilder->setupRenderTarget(renderTarget);
        pipelineBuilder->setupShader(shader);
        pipelineBuilder->setupShapeType(rendererBuilder.getShapeType());
        pipelineBuilder->setupBlendFunctions(rendererBuilder.getBlendFunctions());
        pipelineBuilder->setupDepthOperations(rendererBuilder.isDepthTestEnabled(), rendererBuilder.isDepthWriteEnabled());
        pipelineBuilder->setupCallFaceOperation(rendererBuilder.isCullFaceEnabled());
        pipelineBuilder->setupPolygonMode(rendererBuilder.getPolygonMode());
        pipelineBuilder->setupScissor(rendererBuilder.isScissorEnabled(), rendererBuilder.getScissorOffset(), rendererBuilder.getScissorSize());

        if (renderTarget.isValidRenderTarget()) {
            for (const auto& uniformTextureReaderArray: uniformTextureReaders) {
                for (const auto& uniformTextureReader: uniformTextureReaderArray) {
                    uniformTextureReader->initialize();
                }
            }
        }

        initialize();
    }

    GenericRenderer::~GenericRenderer() {
        cleanup();
        uniformTextureReaders.clear();
        renderTarget.removeRenderer(this);
    }

    void GenericRenderer::initialize() {
        ScopeProfiler sp(Profiler::graphic(), "genRenderInit");
        assert(!isInitialized);

        if (renderTarget.isValidRenderTarget()) {
            createPipeline();
            createVertexBuffers();
            createIndexBuffer();
            createUniformBuffers();
            createDescriptorPool();
            createDescriptorSets();
        }

        isInitialized = true;
    }

    void GenericRenderer::cleanup() {
        if (isInitialized) {
            if (renderTarget.isValidRenderTarget()) {
                vkDeviceWaitIdle(GraphicService::instance().getDevices().getLogicalDevice());

                destroyDescriptorSetsAndPool();
                destroyUniformBuffers();
                destroyIndexBuffer();
                destroyVertexBuffers();
                destroyPipeline();
            }

            isInitialized = false;
        }
    }

    const std::string& GenericRenderer::getName() const {
        return name;
    }

    const RenderTarget& GenericRenderer::getRenderTarget() const {
        return renderTarget;
    }

    bool GenericRenderer::isDrawCommandDirty() const {
        return drawCommandDirty;
    }

    bool GenericRenderer::isEnabled() const {
        return bIsEnabled;
    }

    void GenericRenderer::enableRenderer(unsigned int renderingOrder) {
        assert(!bIsEnabled);
        this->bIsEnabled = true;
        this->renderingOrder = renderingOrder;
        renderTarget.notifyRendererEnabled(this);
    }

    void GenericRenderer::disableRenderer() {
        assert(bIsEnabled);
        bIsEnabled = false;
        renderTarget.notifyRendererDisabled(this);
    }

    unsigned int GenericRenderer::getRenderingOrder() const {
        return renderingOrder;
    }

    bool GenericRenderer::isDepthTestEnabled() const {
        return depthTestEnabled;
    }

    std::size_t GenericRenderer::getPipelineId() const {
        return pipeline->getId();
    }

    void GenericRenderer::createPipeline() {
        pipelineBuilder->setupData(data, instanceData.get());
        pipelineBuilder->setupUniform(uniformData, uniformTextureReaders);
        pipeline = pipelineBuilder->buildPipeline();
    }

    void GenericRenderer::destroyPipeline() {
        pipeline = nullptr;
    }

    void GenericRenderer::createVertexBuffers() {
        vertexBuffers.resize(data.size());
        for (std::size_t dataIndex = 0; dataIndex < data.size(); ++dataIndex) {
            DataContainer& dataContainer = data[dataIndex];
            vertexBuffers[dataIndex].initialize(BufferHandler::VERTEX, BufferHandler::STATIC, renderTarget.getNumFramebuffer(), dataContainer.getBufferSize(), dataContainer.getData());
            dataContainer.markDataAsProcessed();
        }

        if (instanceData) {
            instanceVertexBuffer.initialize(BufferHandler::VERTEX, BufferHandler::STATIC, renderTarget.getNumFramebuffer(), instanceData->getBufferSize(), instanceData->getData());
            instanceData->markDataAsProcessed();
        }
    }

    void GenericRenderer::destroyVertexBuffers() {
        if (instanceData) {
            instanceVertexBuffer.cleanup();
        }

        for (auto& vertexBuffer : vertexBuffers) {
            vertexBuffer.cleanup();
        }
        vertexBuffers.clear();
    }

    void GenericRenderer::createIndexBuffer() {
        if (indices) {
            indexBuffer.initialize(BufferHandler::INDEX, BufferHandler::STATIC, renderTarget.getNumFramebuffer(), indices->getBufferSize(), indices->getIndices());
        }
    }

    void GenericRenderer::destroyIndexBuffer() {
        if (indices) {
            indexBuffer.cleanup();
        }
    }

    void GenericRenderer::createUniformBuffers() {
        uniformsBuffers.resize(uniformData.size());
        for (std::size_t dataIndex = 0; dataIndex < uniformData.size(); ++dataIndex) {
            ShaderDataContainer& shaderDataContainer = uniformData[dataIndex];
            uniformsBuffers[dataIndex].initialize(BufferHandler::UNIFORM, BufferHandler::DYNAMIC, renderTarget.getNumFramebuffer(), shaderDataContainer.getDataSize(), shaderDataContainer.getData());
            shaderDataContainer.markDataAsProcessed();
        }
    }

    void GenericRenderer::destroyUniformBuffers() {
        for (auto& uniformsBuffer : uniformsBuffers) {
            uniformsBuffer.cleanup();
        }
        uniformsBuffers.clear();
    }

    void GenericRenderer::createDescriptorPool() {
        auto logicalDevice = GraphicService::instance().getDevices().getLogicalDevice();

        std::array<VkDescriptorPoolSize, 2> poolSizes{};

        int uboDescriptorCount = std::max(1, (int)renderTarget.getNumFramebuffer() * (int)uniformData.size());
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = (uint32_t)uboDescriptorCount;

        int uniformTexReadersCount = 0;
        std::ranges::for_each(uniformTextureReaders, [&](auto& r){uniformTexReadersCount += (int)r.size();});
        int textureDescriptorCount = std::max(1, (int)renderTarget.getNumFramebuffer() * uniformTexReadersCount);
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = (uint32_t)textureDescriptorCount;

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = (uint32_t)poolSizes.size();
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = (uint32_t)renderTarget.getNumFramebuffer();

        VkResult result = vkCreateDescriptorPool(logicalDevice, &poolInfo, nullptr, &descriptorPool);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor pool with error code: " + std::to_string(result));
        }
    }

    void GenericRenderer::createDescriptorSets() {
        auto logicalDevice = GraphicService::instance().getDevices().getLogicalDevice();

        std::vector<VkDescriptorSetLayout> layouts(renderTarget.getNumFramebuffer(), pipeline->getDescriptorSetLayout());
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = (uint32_t)renderTarget.getNumFramebuffer();
        allocInfo.pSetLayouts = layouts.data();

        descriptorSets.resize(renderTarget.getNumFramebuffer());
        VkResult result = vkAllocateDescriptorSets(logicalDevice, &allocInfo, descriptorSets.data());
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate descriptor sets with error code: " + std::to_string(result));
        }

        updateDescriptorSets();
    }

    void GenericRenderer::updateDescriptorSets() {
        auto logicalDevice = GraphicService::instance().getDevices().getLogicalDevice();

        for (std::size_t frameIndex = 0; frameIndex < renderTarget.getNumFramebuffer(); frameIndex++) {
            std::vector<VkWriteDescriptorSet> descriptorWrites;
            uint32_t shaderUniformBinding = 0;

            //uniform buffer objects
            std::vector<VkDescriptorBufferInfo> bufferInfos;
            for (std::size_t uniformDataIndex = 0; uniformDataIndex < uniformData.size(); ++uniformDataIndex) {
                VkDescriptorBufferInfo bufferInfo{};
                bufferInfo.buffer = uniformsBuffers[uniformDataIndex].getBuffer(frameIndex);
                bufferInfo.offset = 0;
                bufferInfo.range = VK_WHOLE_SIZE;
                bufferInfos.emplace_back(bufferInfo);
            }
            for (const auto& bufferInfo : bufferInfos) {
                VkWriteDescriptorSet uniformDescriptorWrites{};
                uniformDescriptorWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                uniformDescriptorWrites.dstSet = descriptorSets[frameIndex];
                uniformDescriptorWrites.dstBinding = shaderUniformBinding++;
                uniformDescriptorWrites.dstArrayElement = 0;
                uniformDescriptorWrites.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                uniformDescriptorWrites.descriptorCount = 1;
                uniformDescriptorWrites.pBufferInfo = &bufferInfo; //warning: bufferInfo cannot be destroyed before calling vkUpdateDescriptorSets
                descriptorWrites.emplace_back(uniformDescriptorWrites);
            }

            //textures
            std::vector<std::vector<VkDescriptorImageInfo>> imageInfos;
            for (const auto& uniformTextureReaderArray : uniformTextureReaders) {
                std::vector<VkDescriptorImageInfo> imageInfosArray;
                for (const auto& uniformTextureReader : uniformTextureReaderArray) {
                    VkDescriptorImageInfo imageInfo{};
                    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    imageInfo.imageView = uniformTextureReader->getTexture()->getImageView();
                    imageInfo.sampler = uniformTextureReader->getParam().getTextureSampler();
                    imageInfosArray.emplace_back(imageInfo);
                }
                imageInfos.emplace_back(imageInfosArray);
            }
            for (auto& imageInfo : imageInfos) {
                VkWriteDescriptorSet textureDescriptorWrites{};
                textureDescriptorWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                textureDescriptorWrites.dstSet = descriptorSets[frameIndex];
                textureDescriptorWrites.dstBinding = shaderUniformBinding++;
                textureDescriptorWrites.dstArrayElement = 0;
                textureDescriptorWrites.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                textureDescriptorWrites.descriptorCount = (uint32_t)imageInfo.size();
                textureDescriptorWrites.pImageInfo = imageInfo.data(); //warning: imageInfo cannot be destroyed before calling vkUpdateDescriptorSets
                descriptorWrites.emplace_back(textureDescriptorWrites);
            }

            vkUpdateDescriptorSets(logicalDevice, (uint32_t)(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }

        drawCommandDirty = true;
    }

    void GenericRenderer::destroyDescriptorSetsAndPool() {
        vkDestroyDescriptorPool(GraphicService::instance().getDevices().getLogicalDevice(), descriptorPool, nullptr);
    }

    void GenericRenderer::updateData(std::size_t dataIndex, const std::vector<Point2<float>>& dataPtr) {
        #ifdef URCHIN_DEBUG
            assert(data.size() > dataIndex);
            assert(data[dataIndex].getVariableType() == VariableType::VEC2);
            assert(data[dataIndex].getDataType() == DataType::FLOAT);
        #endif
        data[dataIndex].replaceData(dataPtr.size(), dataPtr.data());
    }

    void GenericRenderer::updateData(std::size_t dataIndex, const std::vector<Point3<float>>& dataPtr) {
        #ifdef URCHIN_DEBUG
            assert(data.size() > dataIndex);
            assert(data[dataIndex].getVariableType() == VariableType::VEC3);
            assert(data[dataIndex].getDataType() == DataType::FLOAT);
        #endif
        data[dataIndex].replaceData(dataPtr.size(), dataPtr.data());
    }

    void GenericRenderer::updateData(std::size_t dataIndex, const std::vector<Vector3<float>>& dataPtr) {
        #ifdef URCHIN_DEBUG
            assert(data.size() > dataIndex);
            assert(data[dataIndex].getVariableType() == VariableType::VEC3);
            assert(data[dataIndex].getDataType() == DataType::FLOAT);
        #endif
        data[dataIndex].replaceData(dataPtr.size(), dataPtr.data());
    }

    void GenericRenderer::updateInstanceData(std::size_t instanceCount, const float* dataPtr) {
        #ifdef URCHIN_DEBUG
            assert(instanceData->getDataType() == DataType::FLOAT);
        #endif
        instanceData->replaceData(instanceCount, dataPtr);
    }

    void GenericRenderer::updateUniformData(std::size_t uniformDataIndex, const void* dataPtr) {
        #ifdef URCHIN_DEBUG
            assert(uniformData.size() > uniformDataIndex);
        #endif
        uniformData[uniformDataIndex].updateData(dataPtr);
    }

    void GenericRenderer::updateUniformTextureReader(std::size_t uniformTexPosition, const std::shared_ptr<TextureReader>& textureReader) {
        updateUniformTextureReaderArray(uniformTexPosition, 0, textureReader);
    }

    const std::shared_ptr<TextureReader>& GenericRenderer::getUniformTextureReader(std::size_t uniformTexPosition) const {
        #ifdef URCHIN_DEBUG
            assert(uniformTextureReaders.size() > uniformTexPosition);
            assert(uniformTextureReaders[uniformTexPosition].size() == 1);
        #endif
        return getUniformTextureReaderArray(uniformTexPosition)[0];
    }

    const std::shared_ptr<TextureReader>& GenericRenderer::getUniformTextureReader(std::size_t uniformTexPosition, std::size_t textureIndex) const {
        #ifdef URCHIN_DEBUG
            assert(uniformTextureReaders.size() > uniformTexPosition);
            assert(uniformTextureReaders[uniformTexPosition].size() > textureIndex);
        #endif
        return getUniformTextureReaderArray(uniformTexPosition)[textureIndex];
    }

    void GenericRenderer::updateUniformTextureReaderArray(std::size_t uniformTexPosition, std::size_t textureIndex, const std::shared_ptr<TextureReader>& textureReader) {
        #ifdef URCHIN_DEBUG
            assert(uniformTextureReaders.size() > uniformTexPosition);
            assert(uniformTextureReaders[uniformTexPosition].size() > textureIndex);
        #endif
        vkDeviceWaitIdle(GraphicService::instance().getDevices().getLogicalDevice());

        textureReader->initialize();
        uniformTextureReaders[uniformTexPosition][textureIndex] = textureReader;

        updateDescriptorSets();
    }

    const std::vector<std::shared_ptr<TextureReader>>& GenericRenderer::getUniformTextureReaderArray(std::size_t textureIndex) const {
        #ifdef URCHIN_DEBUG
            assert(uniformTextureReaders.size() > textureIndex);
        #endif
        return uniformTextureReaders[textureIndex];
    }

    const std::vector<OffscreenRender*>& GenericRenderer::getTexturesWriter() const {
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

    void GenericRenderer::updateGraphicData(uint32_t frameIndex) {
        //update data (vertex & vertex attributes)
        #ifdef URCHIN_DEBUG
            std::size_t dataCount = data[0].getDataCount();
            for (std::size_t dataIndex = 1; dataIndex < data.size(); ++dataIndex) {
                assert(indices || dataCount == data[dataIndex].getDataCount());
            }
        #endif
        for (std::size_t dataIndex = 0; dataIndex < data.size(); ++dataIndex) {
            if (data[dataIndex].hasNewData(frameIndex)) {
                DataContainer& dataContainer = data[dataIndex];
                drawCommandDirty |= vertexBuffers[dataIndex].updateData(frameIndex, dataContainer.getBufferSize(), dataContainer.getData());
                dataContainer.markDataAsProcessed(frameIndex);
            }
        }

        //update instance data
        if (instanceData && instanceData->hasNewData(frameIndex)) {
            drawCommandDirty |= instanceVertexBuffer.updateData(frameIndex, instanceData->getBufferSize(), instanceData->getData());
            instanceData->markDataAsProcessed(frameIndex);
        }

        //update shader uniforms
        for (std::size_t uniformDataIndex = 0; uniformDataIndex < uniformData.size(); ++uniformDataIndex) {
            if (uniformData[uniformDataIndex].hasNewData(frameIndex)) {
                const auto& dataContainer = uniformData[uniformDataIndex];
                drawCommandDirty |= uniformsBuffers[uniformDataIndex].updateData(frameIndex, dataContainer.getDataSize(), dataContainer.getData());
                uniformData[uniformDataIndex].markDataAsProcessed(frameIndex);
            }
        }
    }

    std::size_t GenericRenderer::updateCommandBuffer(VkCommandBuffer commandBuffer, std::size_t frameIndex, std::size_t boundPipelineId) {
        ScopeProfiler sp(Profiler::graphic(), "upCmdBufRender");

        if (boundPipelineId != pipeline->getId()) {
            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getGraphicsPipeline());
        }

        std::array<VkDeviceSize, 20> offsets = {0};
        assert(offsets.size() >= data.size());

        rawVertexBuffers.clear();
        for (const auto& vertexBuffer : vertexBuffers) {
            rawVertexBuffers.emplace_back(vertexBuffer.getBuffer(frameIndex));
        }

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getPipelineLayout(), 0, 1, &descriptorSets[frameIndex], 0, nullptr);
        vkCmdBindVertexBuffers(commandBuffer, 0, (uint32_t)data.size(), rawVertexBuffers.data(), offsets.data());
        uint32_t instanceCount = 1;
        if (instanceData) {
            auto firstBinding = (uint32_t)data.size();
            VkBuffer buffer = instanceVertexBuffer.getBuffer(frameIndex);
            vkCmdBindVertexBuffers(commandBuffer, firstBinding, 1, &buffer, offsets.data());
            instanceCount = (uint32_t)instanceData->getDataCount();
        }
        if (indices && indexBuffer.getBuffer(frameIndex)) {
            vkCmdBindIndexBuffer(commandBuffer, indexBuffer.getBuffer(frameIndex), 0, VK_INDEX_TYPE_UINT32);
            vkCmdDrawIndexed(commandBuffer, (uint32_t)indices->getIndicesCount(), instanceCount, 0, 0, 0);
        } else {
            auto vertexCount = (uint32_t)data[0].getDataCount();
            vkCmdDraw(commandBuffer, vertexCount, instanceCount, 0, 0);
        }

        drawCommandDirty = false;
        return pipeline->getId();
    }

}
