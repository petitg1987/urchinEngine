#pragma once

#include <vector>
#include <string>
#include <memory>
#include <vulkan/vulkan.h>

#include "graphics/render/shader/ShaderConstants.h"

namespace urchin {

    struct ShaderStageData {
        VkShaderModule shaderModule;

        VkPipelineShaderStageCreateInfo shaderStageCreateInfo;
        VkSpecializationInfo specializationInfo;
        std::vector<VkSpecializationMapEntry> specializationMapEntries;
    };

    class Shader {
        public:
            enum ShaderType {
                VERTEX,
                FRAGMENT,
                COMPUTE
            };

            Shader(std::size_t, std::string, const std::vector<std::pair<ShaderType, std::vector<char>>>&, std::unique_ptr<ShaderConstants>);
            ~Shader();

            std::size_t getShaderId() const;
            std::vector<VkPipelineShaderStageCreateInfo> getShaderStages() const;

        private:
            void fillShaderModule(ShaderStageData&, const std::vector<char>&) const;
            void fillPipelineShaderStage(ShaderStageData&, ShaderType) const;
            static VkSpecializationMapEntry createSpecializationMapEntry(uint32_t, uint32_t, std::size_t);
            static VkShaderStageFlagBits toShaderStageFlag(ShaderType);

            std::size_t shaderId;
            std::string shaderName;
            std::unique_ptr<ShaderConstants> shaderConstants;

            std::vector<std::unique_ptr<ShaderStageData>> shaderStagesData;
    };

}
