#include <vulkan/vk_enum_string_helper.h>

#include <graphics/api/vulkan/render/GenericCompute.h>
#include <graphics/api/vulkan/setup/GraphicsSetupService.h>
#include <graphics/render/GenericComputeBuilder.h>
#include <graphics/texture/TextureReader.h>

namespace urchin {

    GenericCompute::GenericCompute(const GenericComputeBuilder& computeBuilder) :
            isInitialized(false),
            bIsEnabled(true),
            renderingOrder(0),
            name(computeBuilder.getName()),
            renderTarget(computeBuilder.getRenderTarget()),
            shader(computeBuilder.getShader()),
            uniformTextureReaders(computeBuilder.getUniformTextureReaders()),
            computeDescriptorSetLayout(nullptr),
            computePipelineLayout(nullptr),
            computePipeline(nullptr),
            drawCommandsDirty(false) {

        if (renderTarget.isValidRenderTarget()) {
            for (const auto& uniformTextureReaderArray: uniformTextureReaders) {
                for (const auto& uniformTextureReader: uniformTextureReaderArray) {
                    uniformTextureReader->initialize();
                }
            }
        }

        initialize();
    }

    GenericCompute::~GenericCompute() {
        cleanup();
        uniformTextureReaders.clear();
        renderTarget.removeProcessor(this);
    }

    void GenericCompute::initialize() {
        ScopeProfiler sp(Profiler::graphic(), "genCompInit");
        assert(!isInitialized);

        if (renderTarget.isValidRenderTarget()) {
            createPipeline();
        }
    }

    void GenericCompute::cleanup() {
        if (isInitialized) {
            if (renderTarget.isValidRenderTarget()) {
                VkResult result = vkDeviceWaitIdle(GraphicsSetupService::instance().getDevices().getLogicalDevice());
                if (result != VK_SUCCESS) {
                    Logger::instance().logError("Failed to wait for device idle with error code '" + std::string(string_VkResult(result)) + "' on renderer: " + getName());
                } else {
                    destroyPipeline();
                }
            }

            isInitialized = false;
        }
    }

    const std::string& GenericCompute::getName() const {
        return name;
    }

    const RenderTarget& GenericCompute::getRenderTarget() const {
        return renderTarget;
    }


    bool GenericCompute::needCommandBufferRefresh(std::size_t) const {
        return drawCommandsDirty; //TODO || descriptorSetsDirty[frameIndex];
    }

    bool GenericCompute::isEnabled() const {
        return bIsEnabled;
    }

    void GenericCompute::enableRenderer(unsigned int renderingOrder) {
        this->bIsEnabled = true;
        this->renderingOrder = renderingOrder;
        renderTarget.notifyProcessorEnabled(this);
    }

    void GenericCompute::disableRenderer() {
        bIsEnabled = false;
        renderTarget.notifyProcessorDisabled(this);
    }

    unsigned int GenericCompute::getRenderingOrder() const {
        return renderingOrder;
    }

    bool GenericCompute::isDepthTestEnabled() const {
        return false;
    }

    std::size_t GenericCompute::getPipelineId() const {
        return 123897456; //TODO pipeline->getId();
    }

    std::span<OffscreenRender*> GenericCompute::getTexturesWriter() const {
        return {}; //TODO impl...
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

        VkResult pipelineLayoutResult = vkCreatePipelineLayout(logicalDevice, &pipelineLayoutInfo, nullptr, &computePipelineLayout);
        if (pipelineLayoutResult != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout with error code: " + std::string(string_VkResult(pipelineLayoutResult)));
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

    void GenericCompute::destroyPipeline() {
        auto logicalDevice = GraphicsSetupService::instance().getDevices().getLogicalDevice();

        vkDestroyDescriptorSetLayout(logicalDevice, computeDescriptorSetLayout, nullptr);
        vkDestroyPipeline(logicalDevice, computePipeline, nullptr);
        vkDestroyPipelineLayout(logicalDevice, computePipelineLayout, nullptr);
    }

    void GenericCompute::updateGraphicData(uint32_t) {

    }

    std::size_t GenericCompute::updateCommandBuffer(VkCommandBuffer, std::size_t, std::size_t) {
        ScopeProfiler sp(Profiler::graphic(), "upCmdBufComp");

        drawCommandsDirty = false;
        return 123897456; //TODO ...
    }

}