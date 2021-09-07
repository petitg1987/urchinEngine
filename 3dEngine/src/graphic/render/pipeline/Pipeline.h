#pragma once

#include <vulkan/vulkan.h>

namespace urchin {

    class Pipeline {
        public:
            ~Pipeline();

            VkDescriptorSetLayout descriptorSetLayout;
            VkPipelineLayout pipelineLayout;
            VkPipeline graphicsPipeline;
    };

}
