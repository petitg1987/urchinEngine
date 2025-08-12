#include "libs/vkenum/vk_enum.h"

#include "graphics/api/vulkan/render/GenericCompute.h"
#include "graphics/api/vulkan/setup/GraphicsSetupService.h"
#include "graphics/render/GenericComputeBuilder.h"

namespace urchin {

    GenericCompute::GenericCompute(const GenericComputeBuilder& computeBuilder) :
            PipelineProcessor(computeBuilder.getName(), computeBuilder.getRenderTarget(), computeBuilder.getShader(), computeBuilder.getUniformData(),
                computeBuilder.getUniformTextureReaders(), computeBuilder.getUniformTextureOutputs(), computeBuilder.getStorageBufferData()),
            isInitialized(false),
            threadLocalSize(computeBuilder.getThreadLocalSize()) {

        setupPipelineBuilder(std::make_unique<PipelineBuilder>(COMPUTE, getName()));
        getPipelineBuilder().setupRenderTarget(getRenderTarget());
        getPipelineBuilder().setupShader(getShader());

        initialize();
    }

    GenericCompute::~GenericCompute() {
        cleanup();
    }

    void GenericCompute::initialize() {
        ScopeProfiler sp(Profiler::graphic(), "genCompInit");
        assert(!isInitialized);

        if (!getRenderTarget().isTestMode()) {
            createPipeline();
            createUniformBuffers();
            createStorageBuffers();
            createDescriptorPool();
            createDescriptorSets();
        }

        isInitialized = true;
    }

    void GenericCompute::cleanup() {
        if (isInitialized) {
            if (!getRenderTarget().isTestMode()) {
                VkResult result = vkDeviceWaitIdle(GraphicsSetupService::instance().getDevices().getLogicalDevice());
                if (result != VK_SUCCESS) {
                    Logger::instance().logError("Failed to wait for device idle with error code '" + std::string(string_VkResult(result)) + "' on compute: " + getName());
                } else {
                    destroyDescriptorSetsAndPool();
                    destroyStorageBuffers();
                    destroyUniformBuffers();
                    destroyPipeline();
                }
            }

            isInitialized = false;
        }
    }

    bool GenericCompute::isGraphicsProcessor() const {
        return false;
    }

    bool GenericCompute::isComputeProcessor() const {
        return true;
    }

    bool GenericCompute::isDepthTestEnabled() const {
        return false;
    }

    bool GenericCompute::isApplicableOnLayer(std::size_t) const {
        return true;
    }

    std::size_t GenericCompute::doUpdateCommandBuffer(VkCommandBuffer commandBuffer, std::size_t framebufferIndex, std::size_t, std::size_t currentBoundPipelineId) {
        ScopeProfiler sp(Profiler::graphic(), "upCmdBufComp");

        std::size_t newBoundPipelineId = currentBoundPipelineId;
        if (currentBoundPipelineId != getPipeline().getId()) {
            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, getPipeline().getVkPipeline());
            newBoundPipelineId = getPipeline().getId();
        }

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, getPipeline().getPipelineLayout(), 0, 1, &getDescriptorSets()[framebufferIndex], 0, nullptr);

        auto numGroupX = (uint32_t)MathFunction::ceilToInt((float)getRenderTarget().getWidth() / (float)threadLocalSize.X);
        auto numGroupY = (uint32_t)MathFunction::ceilToInt((float)getRenderTarget().getHeight() / (float)threadLocalSize.Y);
        vkCmdDispatch(commandBuffer, numGroupX, numGroupY, 1);

        return newBoundPipelineId;
    }

}