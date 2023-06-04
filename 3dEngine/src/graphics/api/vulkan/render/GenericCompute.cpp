#include <vulkan/vk_enum_string_helper.h>

#include <graphics/api/vulkan/render/GenericCompute.h>
#include <graphics/api/vulkan/setup/GraphicsSetupService.h>
#include <graphics/render/GenericComputeBuilder.h>
#include <pattern/singleton/Singleton.h>

namespace urchin {

    GenericCompute::GenericCompute(const GenericComputeBuilder& computeBuilder) :
            isInitialized(false),
            bIsEnabled(true),
            renderingOrder(0),
            name(computeBuilder.getName()),
            renderTarget(computeBuilder.getRenderTarget()),
            shader(computeBuilder.getShader()),
            computePipeline(nullptr) {

    }

    void GenericCompute::initialize() {
        ScopeProfiler sp(Profiler::graphic(), "genCompInit");
        assert(!isInitialized);

        if (renderTarget.isValidRenderTarget()) {
            createPipeline();
        }
    }

    void GenericCompute::cleanup() {
        //TODO impl
    }

    void GenericCompute::createPipeline() {
        auto logicalDevice = GraphicsSetupService::instance().getDevices().getLogicalDevice();

        VkComputePipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        pipelineInfo.layout = computePipelineLayout;
        pipelineInfo.stage = shader.getShaderStages()[0];

        VkResult pipelinesResult = vkCreateComputePipelines(logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &computePipeline);
        if (pipelinesResult != VK_SUCCESS) {
            throw std::runtime_error("Failed to create compute pipeline with error code: " + std::string(string_VkResult(pipelinesResult)));
        }
    }

}