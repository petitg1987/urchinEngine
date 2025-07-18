#include <libs/vkenum/vk_enum.h>

#include <graphics/api/vulkan/render/GenericRenderer.h>
#include <graphics/api/vulkan/helper/DebugLabelHelper.h>
#include <graphics/api/vulkan/setup/GraphicsSetupService.h>
#include <graphics/api/vulkan/render/pipeline/PipelineBuilder.h>
#include <graphics/api/vulkan/render/target/RenderTarget.h>
#include <graphics/render/GenericRendererBuilder.h>
#include <graphics/render/data/DataContainer.h>

namespace urchin {

    GenericRenderer::GenericRenderer(const GenericRendererBuilder& rendererBuilder) :
            PipelineProcessor(rendererBuilder.getName(), rendererBuilder.getRenderTarget(), rendererBuilder.getShader(),
                              rendererBuilder.getUniformData(), rendererBuilder.getUniformTextureReaders(), {}),
            isInitialized(false),
            data(rendererBuilder.getData()),
            instanceData(rendererBuilder.getInstanceData()),
            indices(rendererBuilder.getIndices()),
            customScissor(rendererBuilder.hasCustomScissor()),
            scissorOffset(rendererBuilder.getScissorOffset()),
            scissorSize(rendererBuilder.getScissorSize()),
            depthTestEnabled(rendererBuilder.isDepthTestEnabled()),
            layerIndexDataInShaderEnabled(rendererBuilder.isLayerIndexDataInShaderEnabled()),
            layersMask(rendererBuilder.getLayersMask()) {

        setupPipelineBuilder(std::make_unique<PipelineBuilder>(GRAPHICS, getName()));
        getPipelineBuilder().setupRenderTarget(getRenderTarget());
        getPipelineBuilder().setupShader(getShader());
        getPipelineBuilder().setupShapeType(rendererBuilder.getShapeType());
        getPipelineBuilder().setupBlendFunctions(rendererBuilder.getBlendFunctions());
        getPipelineBuilder().setupDepthOperations(rendererBuilder.isDepthTestEnabled(), rendererBuilder.isDepthWriteEnabled());
        getPipelineBuilder().setupCullFaceOperation(rendererBuilder.isCullFaceEnabled());
        getPipelineBuilder().setupLayerIndexDataInShader(rendererBuilder.isLayerIndexDataInShaderEnabled());

        initialize();
    }

    GenericRenderer::~GenericRenderer() {
        cleanup();
    }

    void GenericRenderer::initialize() {
        ScopeProfiler sp(Profiler::graphic(), "genRenderInit");
        assert(!isInitialized);

        if (!getRenderTarget().isTestMode()) {
            createPipeline();
            createDataBuffers();
            createIndexBuffer();
            createUniformBuffers();
            createDescriptorPool();
            createDescriptorSets();
            if (!customScissor) {
                resetScissor();
            }
        }

        isInitialized = true;
    }

    void GenericRenderer::cleanup() {
        if (isInitialized) {
            if (!getRenderTarget().isTestMode()) {
                VkResult result = vkDeviceWaitIdle(GraphicsSetupService::instance().getDevices().getLogicalDevice());
                if (result != VK_SUCCESS) {
                    Logger::instance().logError("Failed to wait for device idle with error code '" + std::string(string_VkResult(result)) + "' on renderer: " + getName());
                } else {
                    destroyDescriptorSetsAndPool();
                    destroyUniformBuffers();
                    destroyIndexBuffer();
                    destroyDataBuffers();
                    destroyPipeline();
                }
            }

            isInitialized = false;
        }
    }

    bool GenericRenderer::isGraphicsProcessor() const {
        return true;
    }

    bool GenericRenderer::isComputeProcessor() const {
        return false;
    }

    bool GenericRenderer::isDepthTestEnabled() const {
        return depthTestEnabled;
    }

    void GenericRenderer::createPipeline() {
        getPipelineBuilder().setupData(data, instanceData.get());
        PipelineProcessor::createPipeline();
    }

    void GenericRenderer::createDataBuffers() {
        dataBuffers.resize(data.size());
        for (std::size_t dataIndex = 0; dataIndex < data.size(); ++dataIndex) {
            DataContainer& dataContainer = data[dataIndex];
            std::string bufferName = getName() + " - data" + std::to_string(dataIndex);
            dataBuffers[dataIndex].initialize(bufferName, BufferHandler::VERTEX, BufferHandler::STATIC, getRenderTarget().getNumFramebuffer(), dataContainer.getBufferSize(), dataContainer.getData());
            dataContainer.markDataAsProcessed();
        }

        if (instanceData) {
            std::string bufferName = getName() + " - instanceData";
            instanceDataBuffer.initialize(bufferName, BufferHandler::VERTEX, BufferHandler::STATIC, getRenderTarget().getNumFramebuffer(), instanceData->getBufferSize(), instanceData->getData());
            instanceData->markDataAsProcessed();
        }
    }

    void GenericRenderer::destroyDataBuffers() {
        if (instanceData) {
            instanceDataBuffer.cleanup();
        }

        for (auto& dataBuffer : dataBuffers) {
            dataBuffer.cleanup();
        }
        dataBuffers.clear();
    }

    void GenericRenderer::createIndexBuffer() {
        if (indices) {
            std::string bufferName = getName() + " - index";
            indexBuffer.initialize(bufferName, BufferHandler::INDEX, BufferHandler::STATIC, getRenderTarget().getNumFramebuffer(), indices->getBufferSize(), indices->getIndices());
        }
    }

    void GenericRenderer::destroyIndexBuffer() {
        if (indices) {
            indexBuffer.cleanup();
        }
    }

    void GenericRenderer::updateData(std::size_t dataIndex, const std::vector<Point2<float>>& dataPtr) {
        #ifdef URCHIN_DEBUG
            assert(data.size() > dataIndex);
        #endif

        if (!getRenderTarget().isTestMode()) {
            data[dataIndex].replaceData(dataPtr.size(), dataPtr.data());
        }
    }

    void GenericRenderer::updateData(std::size_t dataIndex, const std::vector<Point3<float>>& dataPtr) {
        #ifdef URCHIN_DEBUG
            assert(data.size() > dataIndex);
        #endif

        if (!getRenderTarget().isTestMode()) {
            data[dataIndex].replaceData(dataPtr.size(), dataPtr.data());
        }
    }

    void GenericRenderer::updateData(std::size_t dataIndex, const std::vector<Vector3<float>>& dataPtr) {
        #ifdef URCHIN_DEBUG
            assert(data.size() > dataIndex);
        #endif

        if (!getRenderTarget().isTestMode()) {
            data[dataIndex].replaceData(dataPtr.size(), dataPtr.data());
        }
    }

    void GenericRenderer::updateInstanceData(std::size_t instanceCount, const float* dataPtr) const {
        if (!getRenderTarget().isTestMode()) {
            instanceData->replaceData(instanceCount, dataPtr);
        }
    }

    void GenericRenderer::resetScissor() {
        this->customScissor = false;
        this->scissorOffset = Vector2(0, 0);
        this->scissorSize = Vector2((int)getRenderTarget().getWidth(), (int)getRenderTarget().getHeight());
    }

    void GenericRenderer::updateScissor(Vector2<int> scissorOffset, Vector2<int> scissorSize) {
        this->customScissor = true;
        this->scissorOffset = scissorOffset;
        this->scissorSize = scissorSize;
    }

    void GenericRenderer::updateLayersMask(std::bitset<8> layersMask) {
        this->layersMask = layersMask;
    }

    void GenericRenderer::updatePipelineProcessorData(uint32_t framebufferIndex) {
        //update data (vertex & vertex attributes)
        #ifdef URCHIN_DEBUG
            std::size_t dataCount = data[0].getDataCount();
            for (std::size_t dataIndex = 1; dataIndex < data.size(); ++dataIndex) {
                assert(indices || dataCount == data[dataIndex].getDataCount());
            }
        #endif
        for (std::size_t dataIndex = 0; dataIndex < data.size(); ++dataIndex) {
            if (data[dataIndex].hasNewData(framebufferIndex)) {
                DataContainer& dataContainer = data[dataIndex];
                if (dataBuffers[dataIndex].updateData(framebufferIndex, dataContainer.getBufferSize(), dataContainer.getData())) {
                    markDrawCommandsDirty();
                }
                dataContainer.markDataAsProcessed(framebufferIndex);
            }
        }

        //update instance data
        if (instanceData && instanceData->hasNewData(framebufferIndex)) {
            if (instanceDataBuffer.updateData(framebufferIndex, instanceData->getBufferSize(), instanceData->getData())) {
                markDrawCommandsDirty();
            }
            instanceData->markDataAsProcessed(framebufferIndex);
        }

        //update shader uniforms
        PipelineProcessor::updatePipelineProcessorData(framebufferIndex);
    }

    bool GenericRenderer::needCommandBufferRefresh(std::size_t framebufferIndex) const {
        return layerIndexDataInShaderEnabled || PipelineProcessor::needCommandBufferRefresh(framebufferIndex);
    }

    bool GenericRenderer::isApplicableOnLayer(std::size_t layerIndex) const {
        return layersMask.test(layerIndex);
    }

    std::size_t GenericRenderer::doUpdateCommandBuffer(VkCommandBuffer commandBuffer, std::size_t framebufferIndex, std::size_t layerIndex, std::size_t currentBoundPipelineId) {
        ScopeProfiler sp(Profiler::graphic(), "upCmdBufRender");

        std::size_t newBoundPipelineId = currentBoundPipelineId;
        if (currentBoundPipelineId != getPipeline().getId()) {
            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, getPipeline().getVkPipeline());
            newBoundPipelineId = getPipeline().getId();
        }

        if (layerIndexDataInShaderEnabled) {
            int layerIndexInt = (int)layerIndex;
            vkCmdPushConstants(commandBuffer, getPipeline().getPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(int), &layerIndexInt);
        }

        std::array<VkDeviceSize, 20> offsets = {static_cast<VkDeviceSize>(0)};
        assert(offsets.size() >= data.size());

        rawVertexBuffers.clear();
        for (const auto& dataBuffer : dataBuffers) {
            rawVertexBuffers.emplace_back(dataBuffer.getBuffer(framebufferIndex));
        }

        VkRect2D scissor{};
        scissor.offset = {.x = scissorOffset.X, .y = scissorOffset.Y};
        scissor.extent = {.width = (unsigned int)scissorSize.X, .height = (unsigned int)scissorSize.Y};
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, getPipeline().getPipelineLayout(), 0, 1, &getDescriptorSets()[framebufferIndex], 0, nullptr);
        vkCmdBindVertexBuffers(commandBuffer, 0, (uint32_t)data.size(), rawVertexBuffers.data(), offsets.data());
        uint32_t instanceCount = 1;
        if (instanceData) {
            auto firstBinding = (uint32_t)data.size();
            VkBuffer buffer = instanceDataBuffer.getBuffer(framebufferIndex);
            vkCmdBindVertexBuffers(commandBuffer, firstBinding, 1, &buffer, offsets.data());
            instanceCount = (uint32_t)instanceData->getDataCount();
        }
        if (indices && indexBuffer.getBuffer(framebufferIndex)) {
            vkCmdBindIndexBuffer(commandBuffer, indexBuffer.getBuffer(framebufferIndex), 0, VK_INDEX_TYPE_UINT32);
            vkCmdDrawIndexed(commandBuffer, (uint32_t)indices->getIndicesCount(), instanceCount, 0, 0, 0);
        } else {
            auto vertexCount = (uint32_t)data[0].getDataCount();
            vkCmdDraw(commandBuffer, vertexCount, instanceCount, 0, 0);
        }

        return newBoundPipelineId;
    }

}
