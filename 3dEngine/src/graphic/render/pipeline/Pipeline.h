#pragma once

#include <string>
#include <vulkan/vulkan.h>

namespace urchin {

    class Pipeline {
        public:
            Pipeline(std::size_t, std::string);
            ~Pipeline();

            std::size_t getId() const;
            const std::string& getName() const;

            VkDescriptorSetLayout& descriptorSetLayout();
            const VkDescriptorSetLayout& getDescriptorSetLayout() const;

            VkPipelineLayout& pipelineLayout();
            const VkPipelineLayout& getPipelineLayout() const;

            VkPipeline& graphicsPipeline();
            const VkPipeline& getGraphicsPipeline() const;

        private:
            std::size_t id;
            std::string name;

            VkDescriptorSetLayout mDescriptorSetLayout;
            VkPipelineLayout mPipelineLayout;
            VkPipeline mGraphicsPipeline;
    };

}
