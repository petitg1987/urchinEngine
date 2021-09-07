#include <libs/vma/vk_mem_alloc.h>

#include <graphic/render/GenericRenderer.h>
#include <graphic/helper/DebugLabelHelper.h>
#include <graphic/setup/GraphicService.h>
#include <graphic/render/GenericRendererBuilder.h>
#include <graphic/render/model/DataContainer.h>
#include <graphic/render/pipeline/PipelineRetriever.h>

namespace urchin {

    GenericRenderer::GenericRenderer(const GenericRendererBuilder* rendererBuilder) :
            isInitialized(false),
            bIsEnabled(true),
            name(rendererBuilder->getName()),
            renderTarget(rendererBuilder->getRenderTarget()),
            shader(rendererBuilder->getShader()),
            shapeType(rendererBuilder->getShapeType()),
            data(rendererBuilder->getData()),
            indices(rendererBuilder->getIndices()),
            uniformData(rendererBuilder->getUniformData()),
            uniformTextureReaders(rendererBuilder->getUniformTextureReaders()),
            blendFunctions(rendererBuilder->getBlendFunctions()),
            depthTestEnabled(rendererBuilder->isDepthTestEnabled()),
            depthWriteEnabled(rendererBuilder->isDepthWriteEnabled()),
            cullFaceEnabled(rendererBuilder->isCullFaceEnabled()),
            polygonMode(rendererBuilder->getPolygonMode()),
            scissorEnabled(rendererBuilder->isScissorEnabled()),
            scissorOffset(rendererBuilder->getScissorOffset()),
            scissorSize(rendererBuilder->getScissorSize()),
            descriptorPool(nullptr),
            drawCommandDirty(false) {
        if ((depthTestEnabled || depthWriteEnabled) && !renderTarget.hasDepthAttachment()) {
            throw std::runtime_error("Depth operations are enabled but there is no depth attachment on the render target");
        }

        for (auto& uniformTextureReaderArray : uniformTextureReaders) {
            for (auto& uniformTextureReader : uniformTextureReaderArray) {
                uniformTextureReader->initialize();
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
        assert(!isInitialized);

        resetNewDataFlag();
        createPipeline();
        createVertexBuffers();
        createIndexBuffer();
        createUniformBuffers();
        createDescriptorPool();
        createDescriptorSets();

        isInitialized = true;
    }

    void GenericRenderer::cleanup() {
        if (isInitialized) {
            vkDeviceWaitIdle(GraphicService::instance().getDevices().getLogicalDevice());

            destroyDescriptorSetsAndPool();
            destroyUniformBuffers();
            destroyIndexBuffer();
            destroyVertexBuffers();
            destroyPipeline();

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

    void GenericRenderer::enableRenderer() {
        assert(!bIsEnabled);
        bIsEnabled = true;
        renderTarget.notifyRendererEnabled(this);
    }

    void GenericRenderer::disableRenderer() {
        assert(bIsEnabled);
        bIsEnabled = false;
        renderTarget.notifyRendererDisabled(this);
    }

    void GenericRenderer::createPipeline() {
        PipelineRetriever pipelineRetriever;
        pipelineRetriever.setupRenderTarget(renderTarget);
        pipelineRetriever.setupShader(shader);
        pipelineRetriever.setupData(data);
        pipelineRetriever.setupUniform(uniformData, uniformTextureReaders);

        pipeline = pipelineRetriever.getPipeline();
    }

    void GenericRenderer::destroyPipeline() {
        pipeline = nullptr;
    }

    void GenericRenderer::createVertexBuffers() {
        ScopeProfiler sp(Profiler::graphic(), "creVertexBuf");

        vertexBuffers.resize(data.size());
        for (std::size_t dataIndex = 0; dataIndex < data.size(); ++dataIndex) {
            const DataContainer& dataContainer = data[dataIndex];
            vertexBuffers[dataIndex].initialize(BufferHandler::VERTEX, renderTarget.getNumFramebuffer(), dataContainer.getBufferSize(), dataContainer.getData());
        }
    }

    void GenericRenderer::destroyVertexBuffers() {
        for (auto& vertexBuffer : vertexBuffers) {
            vertexBuffer.cleanup();
        }
        vertexBuffers.clear();
    }

    void GenericRenderer::createIndexBuffer() {
        ScopeProfiler sp(Profiler::graphic(), "creIndexBuf");

        if (indices) {
            indexBuffer.initialize(BufferHandler::INDEX, renderTarget.getNumFramebuffer(), indices->getBufferSize(), indices->getIndices());
        }
    }

    void GenericRenderer::destroyIndexBuffer() {
        if (indices) {
            indexBuffer.cleanup();
        }
    }

    void GenericRenderer::createUniformBuffers() {
        ScopeProfiler sp(Profiler::graphic(), "creUniformBuf");

        uniformsBuffers.resize(uniformData.size());
        for (std::size_t dataIndex = 0; dataIndex < uniformData.size(); ++dataIndex) {
            uniformsBuffers[dataIndex].initialize(BufferHandler::UNIFORM, renderTarget.getNumFramebuffer(), uniformData[dataIndex].getDataSize());
        }
    }

    void GenericRenderer::destroyUniformBuffers() {
        for (auto& uniformsBuffer : uniformsBuffers) {
            uniformsBuffer.cleanup();
        }
        uniformsBuffers.clear();
    }

    void GenericRenderer::createDescriptorPool() {
        ScopeProfiler sp(Profiler::graphic(), "creDescPool");
        auto logicalDevice = GraphicService::instance().getDevices().getLogicalDevice();

        std::array<VkDescriptorPoolSize, 2> poolSizes{};

        int uboDescriptorCount = std::max(1, (int)renderTarget.getNumFramebuffer() * (int)uniformData.size());
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = (uint32_t)uboDescriptorCount;

        int uniformTexReadersCount = 0;
        std::for_each(uniformTextureReaders.begin(), uniformTextureReaders.end(), [&] (auto& r){uniformTexReadersCount += (int)r.size();});
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
        ScopeProfiler sp(Profiler::graphic(), "creDescSet");
        auto logicalDevice = GraphicService::instance().getDevices().getLogicalDevice();

        std::vector<VkDescriptorSetLayout> layouts(renderTarget.getNumFramebuffer(), pipeline->descriptorSetLayout);
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
        ScopeProfiler sp(Profiler::graphic(), "updateDescSet");
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
            for (auto& bufferInfo : bufferInfos) {
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
            for (auto& uniformTextureReaderArray : uniformTextureReaders) {
                std::vector<VkDescriptorImageInfo> imageInfosArray;
                for (auto& uniformTextureReader : uniformTextureReaderArray) {
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

    void GenericRenderer::resetNewDataFlag() {
        for (auto& singleData : data) {
            singleData.resetNewDataFlag();
        }

        for (auto& singleUniformData : uniformData) {
            singleUniformData.resetNewDataFlag();
        }
    }

    void GenericRenderer::updateData(std::size_t dataIndex, const std::vector<Point2<float>>& dataPtr) {
        DataContainer dataContainer(DataType::FLOAT, DataDimension::TWO_DIMENSION, dataPtr.size(), dataPtr.data());
        updateData(dataIndex, std::move(dataContainer));
    }

    void GenericRenderer::updateData(std::size_t dataIndex, const std::vector<Point3<float>>& dataPtr) {
        DataContainer dataContainer(DataType::FLOAT, DataDimension::THREE_DIMENSION, dataPtr.size(), dataPtr.data());
        updateData(dataIndex, std::move(dataContainer));
    }

    void GenericRenderer::updateData(std::size_t dataIndex, const std::vector<Vector3<float>>& dataPtr) {
        DataContainer dataContainer(DataType::FLOAT, DataDimension::THREE_DIMENSION, dataPtr.size(), dataPtr.data());
        updateData(dataIndex, std::move(dataContainer));
    }

    void GenericRenderer::updateData(std::size_t dataIndex, DataContainer&& dataContainer) {
        assert(data.size() > dataIndex);
        data[dataIndex] = std::move(dataContainer);
    }

    void GenericRenderer::updateUniformData(std::size_t uniformDataIndex, const void* dataPtr) {
        assert(uniformData.size() > uniformDataIndex);
        uniformData[uniformDataIndex].updateData(dataPtr);
    }

    void GenericRenderer::updateUniformTextureReader(std::size_t uniformTexPosition, const std::shared_ptr<TextureReader>& textureReader) {
        updateUniformTextureReaderArray(uniformTexPosition, 0, textureReader);
    }

    const std::shared_ptr<TextureReader>& GenericRenderer::getUniformTextureReader(std::size_t uniformTexPosition) const {
        assert(uniformTextureReaders.size() > uniformTexPosition);
        assert(uniformTextureReaders[uniformTexPosition].size() == 1);
        return getUniformTextureReaderArray(uniformTexPosition)[0];
    }

    const std::shared_ptr<TextureReader>& GenericRenderer::getUniformTextureReader(std::size_t uniformTexPosition, std::size_t textureIndex) const {
        assert(uniformTextureReaders.size() > uniformTexPosition);
        assert(uniformTextureReaders[uniformTexPosition].size() > textureIndex);
        return getUniformTextureReaderArray(uniformTexPosition)[textureIndex];
    }

    void GenericRenderer::updateUniformTextureReaderArray(std::size_t uniformTexPosition, std::size_t textureIndex, const std::shared_ptr<TextureReader>& textureReader) {
        assert(uniformTextureReaders.size() > uniformTexPosition);
        assert(uniformTextureReaders[uniformTexPosition].size() > textureIndex);

        vkDeviceWaitIdle(GraphicService::instance().getDevices().getLogicalDevice());

        textureReader->initialize();
        uniformTextureReaders[uniformTexPosition][textureIndex] = textureReader;

        updateDescriptorSets();
    }

    const std::vector<std::shared_ptr<TextureReader>>& GenericRenderer::getUniformTextureReaderArray(std::size_t textureIndex) const {
        assert(uniformTextureReaders.size() > textureIndex);
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
        #ifndef NDEBUG
            std::size_t dataCount = data[0].getDataCount();
            for (std::size_t dataIndex = 1; dataIndex < data.size(); ++dataIndex) {
                assert(indices || dataCount == data[dataIndex].getDataCount());
            }
        #endif
        for (std::size_t dataIndex = 0; dataIndex < data.size(); ++dataIndex) {
            if (data[dataIndex].hasNewData(frameIndex)) {
                auto& dataContainer = data[dataIndex];
                drawCommandDirty |= vertexBuffers[dataIndex].updateData(frameIndex, dataContainer.getBufferSize(), dataContainer.getData());
                data[dataIndex].newDataAck(frameIndex);
            }
        }

        //update shader uniforms
        for (std::size_t uniformDataIndex = 0; uniformDataIndex < uniformData.size(); ++uniformDataIndex) {
            if (uniformData[uniformDataIndex].hasNewData(frameIndex)) {
                auto& dataContainer = uniformData[uniformDataIndex];
                drawCommandDirty |= uniformsBuffers[uniformDataIndex].updateData(frameIndex, dataContainer.getDataSize(), dataContainer.getData());
                uniformData[uniformDataIndex].newDataAck(frameIndex);
            }
        }
    }

    void GenericRenderer::updateCommandBuffer(VkCommandBuffer commandBuffer, std::size_t frameIndex) {
        ScopeProfiler sp(Profiler::graphic(), "updateCmdBuf");
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->graphicsPipeline);

        std::array<VkDeviceSize, 20> offsets = {0};
        assert(offsets.size() >= data.size());

        rawVertexBuffers.clear();
        for (const auto& vertexBuffer : vertexBuffers) {
            rawVertexBuffers.emplace_back(vertexBuffer.getBuffer(frameIndex));
        }

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipelineLayout, 0, 1, &descriptorSets[frameIndex], 0, nullptr);
        vkCmdBindVertexBuffers(commandBuffer, 0, (uint32_t)data.size(), rawVertexBuffers.data(), offsets.data());
        if (indices && indexBuffer.getBuffer(frameIndex)) {
            vkCmdBindIndexBuffer(commandBuffer, indexBuffer.getBuffer(frameIndex), 0, VK_INDEX_TYPE_UINT32);
            vkCmdDrawIndexed(commandBuffer, (uint32_t)indices->getIndicesCount(), 1, 0, 0, 0);
        } else {
            auto vertexCount = (uint32_t)data[0].getDataCount();
            vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
        }

        drawCommandDirty = false;
    }

}
