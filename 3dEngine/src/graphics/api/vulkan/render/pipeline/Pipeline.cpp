#include <graphics/api/vulkan/render/pipeline/Pipeline.h>
#include <graphics/api/vulkan/setup/GraphicsSetupService.h>

namespace urchin {

    Pipeline::Pipeline(PipelineType type, std::size_t id, std::string name) :
            type(type),
            id(id),
            name(std::move(name)),
            mDescriptorSetLayout(nullptr),
            mPipelineLayout(nullptr),
            mPipeline(nullptr) {

    }

    Pipeline::~Pipeline() {
        auto logicalDevice = GraphicsSetupService::instance().getDevices().getLogicalDevice();

        vkDestroyDescriptorSetLayout(logicalDevice, mDescriptorSetLayout, nullptr);
        vkDestroyPipeline(logicalDevice, mPipeline, nullptr);
        vkDestroyPipelineLayout(logicalDevice, mPipelineLayout, nullptr);
    }

    PipelineType Pipeline::getType() const {
        return type;
    }

    std::size_t Pipeline::getId() const {
        return id;
    }

    const std::string& Pipeline::getName() const {
        return name;
    }

    VkDescriptorSetLayout& Pipeline::descriptorSetLayout() {
        return mDescriptorSetLayout;
    }

    const VkDescriptorSetLayout& Pipeline::getDescriptorSetLayout() const {
        return mDescriptorSetLayout;
    }

    VkPipelineLayout& Pipeline::pipelineLayout() {
        return mPipelineLayout;
    }

    const VkPipelineLayout& Pipeline::getPipelineLayout() const {
        return mPipelineLayout;
    }

    VkPipeline& Pipeline::pipeline() {
        return mPipeline;
    }

    const VkPipeline& Pipeline::getPipeline() const {
        return mPipeline;
    }

}
