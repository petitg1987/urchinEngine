#include "Shader.h"
#include "graphic/setup/GraphicService.h"

namespace urchin {

    Shader::Shader(std::string shaderName, const std::vector<std::pair<Shader::ShaderType, std::vector<char>>>& shaderSources):
            shaderName(std::move(shaderName)) {
        for(const auto& shaderSource : shaderSources) {
            VkShaderModule shaderModule = createShaderModule(shaderSource.second);
            shaderModules.emplace_back(shaderModule);

            shaderStages.emplace_back(createPipelineShaderStage(shaderModule, shaderSource.first));
        }
    }

    Shader::~Shader() {
        auto logicalDevice = GraphicService::instance()->getDevices().getLogicalDevice();
        for(auto shaderModule : shaderModules) {
            vkDestroyShaderModule(logicalDevice, shaderModule, nullptr);
        }
    }

    const std::vector<VkPipelineShaderStageCreateInfo>& Shader::getShaderStages() const {
        return shaderStages;
    }

    VkShaderModule Shader::createShaderModule(const std::vector<char>& shaderCode) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = shaderCode.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

        VkShaderModule shaderModule;
        VkResult result = vkCreateShaderModule(GraphicService::instance()->getDevices().getLogicalDevice(), &createInfo, nullptr, &shaderModule);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create shader module with error code: " + std::to_string(result));
        }
        return shaderModule;
    }

    VkPipelineShaderStageCreateInfo Shader::createPipelineShaderStage(VkShaderModule shaderModule, ShaderType shaderType) {
        VkPipelineShaderStageCreateInfo shaderStageInfo{};
        shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStageInfo.stage = toShaderStageFlag(shaderType);
        shaderStageInfo.module = shaderModule;
        shaderStageInfo.pName = "main";
        shaderStageInfo.pSpecializationInfo = nullptr; //can be used to transfer constant variables to the shader

        return shaderStageInfo;
    }

    VkShaderStageFlagBits Shader::toShaderStageFlag(Shader::ShaderType shaderType) {
        if (shaderType == Shader::ShaderType::VERTEX) {
            return VK_SHADER_STAGE_VERTEX_BIT;
        } else if (shaderType == Shader::ShaderType::GEOMETRY) {
            return VK_SHADER_STAGE_GEOMETRY_BIT;
        } else if (shaderType == Shader::ShaderType::FRAGMENT) {
            return VK_SHADER_STAGE_FRAGMENT_BIT;
        }
        throw std::invalid_argument("Unknown shader type: " + std::to_string(shaderType));
    }

}
