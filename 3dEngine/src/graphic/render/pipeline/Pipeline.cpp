#include <graphic/render/pipeline/Pipeline.h>
#include <graphic/setup/GraphicService.h>

namespace urchin {

    Pipeline::Pipeline(std::string name) :
            name(std::move(name)),
            descriptorSetLayout(nullptr),
            pipelineLayout(nullptr),
            graphicsPipeline(nullptr) {

    }

    Pipeline::~Pipeline() {
        auto logicalDevice = GraphicService::instance().getDevices().getLogicalDevice();

        vkDestroyDescriptorSetLayout(logicalDevice, descriptorSetLayout, nullptr);
        vkDestroyPipeline(logicalDevice, graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(logicalDevice, pipelineLayout, nullptr);
    }

}
