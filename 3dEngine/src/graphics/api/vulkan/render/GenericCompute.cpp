#include <vulkan/generated/vk_enum_string_helper.h>

#include <graphics/api/vulkan/render/GenericCompute.h>
#include <graphics/api/vulkan/setup/GraphicsSetupService.h>
#include <graphics/render/GenericComputeBuilder.h>

namespace urchin {

    GenericCompute::GenericCompute(const GenericComputeBuilder& computeBuilder) :
            PipelineProcessor(computeBuilder.getName(), computeBuilder.getRenderTarget(), computeBuilder.getShader(),
                              computeBuilder.getUniformData(), computeBuilder.getUniformTextureReaders(), computeBuilder.getUniformTextureOutputs()),
            isInitialized(false),
            threadLocalSize(computeBuilder.getThreadLocalSize()) {

        setupPipelineBuilder(std::make_unique<PipelineBuilder>(PipelineType::COMPUTE, getName()));
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

        if (getRenderTarget().isValidRenderTarget()) {
            createPipeline();
            createUniformBuffers();
            createDescriptorPool();
            createDescriptorSets();
        }

        isInitialized = true;
    }

    void GenericCompute::cleanup() {
        if (isInitialized) {
            if (getRenderTarget().isValidRenderTarget()) {
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

    bool GenericCompute::isGraphicsPipeline() const {
        return false;
    }

    bool GenericCompute::isDepthTestEnabled() const {
        return false;
    }

    void GenericCompute::doUpdateCommandBuffer(VkCommandBuffer commandBuffer, std::size_t frameIndex, std::size_t boundPipelineId) {
        ScopeProfiler sp(Profiler::graphic(), "upCmdBufComp");

        if (boundPipelineId != getPipeline().getId()) {
            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, getPipeline().getVkPipeline());
        }

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, getPipeline().getPipelineLayout(), 0, 1, &getDescriptorSets()[frameIndex], 0, nullptr);

        auto numGroupX = (uint32_t)MathFunction::ceilToInt((float)getRenderTarget().getWidth() / (float)threadLocalSize.X);
        auto numGroupY = (uint32_t)MathFunction::ceilToInt((float)getRenderTarget().getHeight() / (float)threadLocalSize.Y);
        vkCmdDispatch(commandBuffer, numGroupX, numGroupY, 1);
    }

}