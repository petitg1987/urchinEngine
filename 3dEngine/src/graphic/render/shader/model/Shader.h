#ifndef URCHINENGINE_SHADER_H
#define URCHINENGINE_SHADER_H

#include <vector>
#include <string>
#include <vulkan/vulkan.h>

namespace urchin {

    class Shader {
        public:
            enum ShaderType {
                VERTEX,
                GEOMETRY,
                FRAGMENT
            };

            Shader(std::string, const std::vector<std::pair<Shader::ShaderType, std::vector<char>>>&);
            ~Shader();

            const std::vector<VkPipelineShaderStageCreateInfo>& getShaderStages() const;

        private:
            static VkShaderModule createShaderModule(const std::vector<char>&);
            static VkPipelineShaderStageCreateInfo createPipelineShaderStage(VkShaderModule, ShaderType);
            static VkShaderStageFlagBits toShaderStageFlag(Shader::ShaderType);

            std::string shaderName;
            std::vector<VkShaderModule> shaderModules;
            std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
    };

}

#endif
