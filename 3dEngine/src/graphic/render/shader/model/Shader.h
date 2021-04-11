#ifndef URCHINENGINE_SHADER_H
#define URCHINENGINE_SHADER_H

#include <vector>
#include <string>
#include <memory>
#include <vulkan/vulkan.h>

#include "ShaderConstants.h"

namespace urchin {

    struct ShaderStageData {
        VkPipelineShaderStageCreateInfo shaderStageCreateInfo;
        VkSpecializationInfo specializationInfo;
        std::vector<VkSpecializationMapEntry> specializationMapEntries;
    };

    class Shader {
        public:
            enum ShaderType {
                VERTEX,
                GEOMETRY,
                FRAGMENT
            };

            Shader(std::string, const std::vector<std::pair<Shader::ShaderType, std::vector<char>>>&);
            Shader(std::string, const std::vector<std::pair<Shader::ShaderType, std::vector<char>>>&, std::unique_ptr<ShaderConstants>);
            ~Shader();

            std::vector<VkPipelineShaderStageCreateInfo> getShaderStages() const;

        private:
            static VkShaderModule createShaderModule(const std::vector<char>&);
            std::unique_ptr<ShaderStageData> createPipelineShaderStage(VkShaderModule, ShaderType);
            static VkSpecializationMapEntry createSpecializationMapEntry(uint32_t, uint32_t, std::size_t);
            static VkShaderStageFlagBits toShaderStageFlag(Shader::ShaderType);

            std::string shaderName;
            std::unique_ptr<ShaderConstants> shaderConstants;

            std::vector<VkShaderModule> shaderModules;
            std::vector<std::unique_ptr<ShaderStageData>> shaderStagesData;
    };

}

#endif
