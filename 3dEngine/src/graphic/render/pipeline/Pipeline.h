#pragma once

#include <string>
#include <vulkan/vulkan.h>

namespace urchin {

    class Pipeline {
        public:
            explicit Pipeline(std::string);
            ~Pipeline();

            const std::string& getName() const;

            VkDescriptorSetLayout& descriptorSetLayout();
            VkPipelineLayout& pipelineLayout();
            VkPipeline& graphicsPipeline();

        private:
            std::string name;

            VkDescriptorSetLayout mDescriptorSetLayout;
            VkPipelineLayout mPipelineLayout;
            VkPipeline mGraphicsPipeline;
    };

}
