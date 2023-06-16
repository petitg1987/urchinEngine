#pragma once

#include <string>
#include <vulkan/vulkan.h>

namespace urchin {

    enum PipelineType {
        GRAPHICS,
        COMPUTE
    };

    class Pipeline {
        public:
            Pipeline(PipelineType, std::size_t, std::string);
            ~Pipeline();

            PipelineType getType() const;
            std::size_t getId() const;
            const std::string& getName() const;

            VkDescriptorSetLayout& descriptorSetLayout();
            const VkDescriptorSetLayout& getDescriptorSetLayout() const;

            VkPipelineLayout& pipelineLayout();
            const VkPipelineLayout& getPipelineLayout() const;

            VkPipeline& vkPipeline();
            const VkPipeline& getVkPipeline() const;

        private:
            PipelineType type;
            std::size_t id;
            std::string name;

            VkDescriptorSetLayout mDescriptorSetLayout;
            VkPipelineLayout mPipelineLayout;
            VkPipeline mPipeline;
    };

}
