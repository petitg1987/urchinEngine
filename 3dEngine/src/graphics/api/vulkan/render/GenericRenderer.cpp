#include <libs/vkenum/vk_enum.h>
#include <libs/vma/vk_mem_alloc.h>

#include <graphics/api/vulkan/render/GenericRenderer.h>
#include <graphics/api/vulkan/helper/DebugLabelHelper.h>
#include <graphics/api/vulkan/setup/GraphicsSetupService.h>
#include <graphics/api/vulkan/render/pipeline/PipelineBuilder.h>
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
            depthTestEnabled(rendererBuilder.isDepthTestEnabled()) {

        setupPipelineBuilder(std::make_unique<PipelineBuilder>(PipelineType::GRAPHICS, getName()));
        getPipelineBuilder().setupRenderTarget(getRenderTarget());
        getPipelineBuilder().setupShader(getShader());
        getPipelineBuilder().setupShapeType(rendererBuilder.getShapeType());
        getPipelineBuilder().setupBlendFunctions(rendererBuilder.getBlendFunctions());
        getPipelineBuilder().setupDepthOperations(rendererBuilder.isDepthTestEnabled(), rendererBuilder.isDepthWriteEnabled());
        getPipelineBuilder().setupCullFaceOperation(rendererBuilder.isCullFaceEnabled());
        getPipelineBuilder().setupPolygonMode(rendererBuilder.getPolygonMode());

        initialize();
    }

    GenericRenderer::~GenericRenderer() {
        cleanup();
    }

    void GenericRenderer::initialize() {
        ScopeProfiler sp(Profiler::graphic(), "genRenderInit");
        assert(!isInitialized);

        if (getRenderTarget().isValidRenderTarget()) {
            createPipeline();
            createVertexBuffers();
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
            if (getRenderTarget().isValidRenderTarget()) {
                VkResult result = vkDeviceWaitIdle(GraphicsSetupService::instance().getDevices().getLogicalDevice());
                if (result != VK_SUCCESS) {
                    Logger::instance().logError("Failed to wait for device idle with error code '" + std::string(string_VkResult(result)) + "' on renderer: " + getName());
                } else {
                    destroyDescriptorSetsAndPool();
                    destroyUniformBuffers();
                    destroyIndexBuffer();
                    destroyVertexBuffers();
                    destroyPipeline();
                }
            }

            isInitialized = false;
        }
    }

    bool GenericRenderer::isGraphicsPipeline() const {
        return true;
    }

    bool GenericRenderer::isDepthTestEnabled() const {
        return depthTestEnabled;
    }

    void GenericRenderer::createPipeline() {
        getPipelineBuilder().setupData(data, instanceData.get());
        PipelineProcessor::createPipeline();
    }

    void GenericRenderer::createVertexBuffers() {
        vertexBuffers.resize(data.size());
        for (std::size_t dataIndex = 0; dataIndex < data.size(); ++dataIndex) {
            DataContainer& dataContainer = data[dataIndex];
            std::string bufferName = getName() + " - vertex" + std::to_string(dataIndex);
            vertexBuffers[dataIndex].initialize(bufferName, BufferHandler::VERTEX, BufferHandler::STATIC, getRenderTarget().getNumFramebuffer(), dataContainer.getBufferSize(), dataContainer.getData());
            dataContainer.markDataAsProcessed();
        }

        if (instanceData) {
            std::string bufferName = getName() + " - instance";
            instanceVertexBuffer.initialize(bufferName, BufferHandler::VERTEX, BufferHandler::STATIC, getRenderTarget().getNumFramebuffer(), instanceData->getBufferSize(), instanceData->getData());
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
            assert(data[dataIndex].getVariableType() == VariableType::VEC2);
            assert(data[dataIndex].getDataType() == DataType::FLOAT);
        #endif

        if (getRenderTarget().isValidRenderTarget()) {
            data[dataIndex].replaceData(dataPtr.size(), dataPtr.data());
        }
    }

    void GenericRenderer::updateData(std::size_t dataIndex, const std::vector<Point3<float>>& dataPtr) {
        #ifdef URCHIN_DEBUG
            assert(data.size() > dataIndex);
            assert(data[dataIndex].getVariableType() == VariableType::VEC3);
            assert(data[dataIndex].getDataType() == DataType::FLOAT);
        #endif

        if (getRenderTarget().isValidRenderTarget()) {
            data[dataIndex].replaceData(dataPtr.size(), dataPtr.data());
        }
    }

    void GenericRenderer::updateData(std::size_t dataIndex, const std::vector<Vector3<float>>& dataPtr) {
        #ifdef URCHIN_DEBUG
            assert(data.size() > dataIndex);
            assert(data[dataIndex].getVariableType() == VariableType::VEC3);
            assert(data[dataIndex].getDataType() == DataType::FLOAT);
        #endif

        if (getRenderTarget().isValidRenderTarget()) {
            data[dataIndex].replaceData(dataPtr.size(), dataPtr.data());
        }
    }

    void GenericRenderer::updateInstanceData(std::size_t instanceCount, const float* dataPtr) const {
        #ifdef URCHIN_DEBUG
            assert(instanceData->getDataType() == DataType::FLOAT);
        #endif

        if (getRenderTarget().isValidRenderTarget()) {
            instanceData->replaceData(instanceCount, dataPtr);
        }
    }

    void GenericRenderer::resetScissor() {
        this->customScissor = false;
        this->scissorOffset = Vector2<int>(0, 0);
        this->scissorSize = Vector2<int>((int)getRenderTarget().getWidth(), (int)getRenderTarget().getHeight());
    }

    void GenericRenderer::updateScissor(Vector2<int> scissorOffset, Vector2<int> scissorSize) {
        this->customScissor = true;
        this->scissorOffset = scissorOffset;
        this->scissorSize = scissorSize;
    }

    void GenericRenderer::updatePipelineProcessorData(uint32_t frameIndex) {
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
                if (vertexBuffers[dataIndex].updateData(frameIndex, dataContainer.getBufferSize(), dataContainer.getData())) {
                    markDrawCommandsDirty();
                }
                dataContainer.markDataAsProcessed(frameIndex);
            }
        }

        //update instance data
        if (instanceData && instanceData->hasNewData(frameIndex)) {
            if (instanceVertexBuffer.updateData(frameIndex, instanceData->getBufferSize(), instanceData->getData())) {
                markDrawCommandsDirty();
            }
            instanceData->markDataAsProcessed(frameIndex);
        }

        //update shader uniforms
        PipelineProcessor::updatePipelineProcessorData(frameIndex);
    }

    void GenericRenderer::doUpdateCommandBuffer(VkCommandBuffer commandBuffer, std::size_t frameIndex, std::size_t boundPipelineId) {
        ScopeProfiler sp(Profiler::graphic(), "upCmdBufRender");

        if (boundPipelineId != getPipeline().getId()) {
            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, getPipeline().getVkPipeline());
        }

        std::array<VkDeviceSize, 20> offsets = {0};
        assert(offsets.size() >= data.size());

        rawVertexBuffers.clear();
        for (const auto& vertexBuffer : vertexBuffers) {
            rawVertexBuffers.emplace_back(vertexBuffer.getBuffer(frameIndex));
        }

        VkRect2D scissor = {};
        scissor.offset = {.x = scissorOffset.X, .y = scissorOffset.Y};
        scissor.extent = {.width = (unsigned int)scissorSize.X, .height = (unsigned int)scissorSize.Y};
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, getPipeline().getPipelineLayout(), 0, 1, &getDescriptorSets()[frameIndex], 0, nullptr);
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
    }

}
