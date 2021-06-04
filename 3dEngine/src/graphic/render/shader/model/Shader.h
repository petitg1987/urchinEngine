#pragma once

#include <vector>
#include <string>
#include <memory>
#include <vulkan/vulkan.h>

#include <graphic/render/shader/model/ShaderConstants.h>

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
                GEOMETRY,
                FRAGMENT
            };

            Shader(std::string, const std::vector<std::pair<Shader::ShaderType, std::vector<char>>>&, std::unique_ptr<ShaderConstants>);
            ~Shader();

            std::vector<VkPipelineShaderStageCreateInfo> getShaderStages() const;

        private:
            void fillShaderModule(const std::unique_ptr<ShaderStageData>&, const std::vector<char>&) const;
            void fillPipelineShaderStage(const std::unique_ptr<ShaderStageData>&, ShaderType) const;
            static VkSpecializationMapEntry createSpecializationMapEntry(uint32_t, uint32_t, std::size_t);
            static VkShaderStageFlagBits toShaderStageFlag(Shader::ShaderType);

            std::string shaderName;
            std::unique_ptr<ShaderConstants> shaderConstants;

            std::vector<std::unique_ptr<ShaderStageData>> shaderStagesData;
    };

}
