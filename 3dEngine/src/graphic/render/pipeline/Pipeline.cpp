#include <graphic/render/pipeline/Pipeline.h>
#include <graphic/setup/GraphicService.h>

namespace urchin {

    Pipeline::Pipeline(std::string name) :
            name(std::move(name)),
            mDescriptorSetLayout(nullptr),
            mPipelineLayout(nullptr),
            mGraphicsPipeline(nullptr) {

    }

    Pipeline::~Pipeline() {
        auto logicalDevice = GraphicService::instance().getDevices().getLogicalDevice();

        vkDestroyDescriptorSetLayout(logicalDevice, mDescriptorSetLayout, nullptr);
        vkDestroyPipeline(logicalDevice, mGraphicsPipeline, nullptr);
        vkDestroyPipelineLayout(logicalDevice, mPipelineLayout, nullptr);
    }

    const std::string& Pipeline::getName() const {
        return name;
    }

    VkDescriptorSetLayout& Pipeline::descriptorSetLayout() {
        return mDescriptorSetLayout;
    }

    VkPipelineLayout& Pipeline::pipelineLayout() {
        return mPipelineLayout;
    }

    VkPipeline& Pipeline::graphicsPipeline() {
        return mGraphicsPipeline;
    }

}
