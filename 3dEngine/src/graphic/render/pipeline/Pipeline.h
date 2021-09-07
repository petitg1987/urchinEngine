#pragma once

#include <string>
#include <vulkan/vulkan.h>

namespace urchin {

    class Pipeline { //TODO getter / setter ?
        public:
            explicit Pipeline(std::string);
            ~Pipeline();

            std::string name;

            VkDescriptorSetLayout descriptorSetLayout;
            VkPipelineLayout pipelineLayout;
            VkPipeline graphicsPipeline;
    };

}
