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
            uniformTextureReaders(computeBuilder.getUniformTextureReaders()),
            computePipeline(nullptr) {

        if (renderTarget.isValidRenderTarget()) {
            for (const auto& uniformTextureReaderArray: uniformTextureReaders) {
                for (const auto& uniformTextureReader: uniformTextureReaderArray) {
                    uniformTextureReader->initialize();
                }
            }
        }
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

        uint32_t shaderUniformBinding = 0;
        std::vector<VkDescriptorSetLayoutBinding> bindings;

        for (auto& uniformTextureReader : uniformTextureReaders) {
            VkDescriptorSetLayoutBinding samplerLayoutBinding{};
            samplerLayoutBinding.binding = shaderUniformBinding++;
            samplerLayoutBinding.descriptorCount = (uint32_t)uniformTextureReader.size();
            samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; //TODO or VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER ?
            samplerLayoutBinding.pImmutableSamplers = nullptr;
            samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
            bindings.emplace_back(samplerLayoutBinding);
        }

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = (uint32_t)bindings.size();
        layoutInfo.pBindings = bindings.empty() ? nullptr : bindings.data();

        VkResult result = vkCreateDescriptorSetLayout(logicalDevice, &layoutInfo, nullptr, &computeDescriptorSetLayout);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor set layout with error code: " + std::string(string_VkResult(result)));
        }

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &computeDescriptorSetLayout;

        if (vkCreatePipelineLayout(logicalDevice, &pipelineLayoutInfo, nullptr, &computePipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create compute pipeline layout!");
        }

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