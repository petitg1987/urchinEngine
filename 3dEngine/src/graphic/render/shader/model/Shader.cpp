#include "Shader.h"
#include "graphic/setup/GraphicService.h"
#include "graphic/helper/ObjectNamingHelper.h"

namespace urchin {

    Shader::Shader(std::string shaderName, const std::vector<std::pair<Shader::ShaderType, std::vector<char>>>& shaderSources):
            shaderName(std::move(shaderName)),
            shaderConstants(std::unique_ptr<ShaderConstants>(nullptr)) {
        for(const auto& shaderSource : shaderSources) {
            auto shaderStageData = std::make_unique<ShaderStageData>();

            fillShaderModule(shaderStageData, shaderSource.second);
            fillPipelineShaderStage(shaderStageData, shaderSource.first);

            shaderStagesData.emplace_back(std::move(shaderStageData));
        }
    }

    Shader::Shader(std::string shaderName, const std::vector<std::pair<Shader::ShaderType, std::vector<char>>>& shaderSources, std::unique_ptr<ShaderConstants> shaderConstants) :
            shaderName(std::move(shaderName)),
            shaderConstants(std::move(shaderConstants)) {
        for(const auto& shaderSource : shaderSources) {
            auto shaderStageData = std::make_unique<ShaderStageData>();

            fillShaderModule(shaderStageData, shaderSource.second);
            fillPipelineShaderStage(shaderStageData, shaderSource.first);

            shaderStagesData.emplace_back(std::move(shaderStageData));
        }
    }

    Shader::~Shader() {
        auto logicalDevice = GraphicService::instance()->getDevices().getLogicalDevice();
        for(auto& shaderStageData : shaderStagesData) {
            vkDestroyShaderModule(logicalDevice, shaderStageData->shaderModule, nullptr);
        }
    }

    std::vector<VkPipelineShaderStageCreateInfo> Shader::getShaderStages() const {
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
        shaderStages.reserve(shaderStagesData.size());

        for(auto& shaderStageData : shaderStagesData) {
            shaderStages.emplace_back(shaderStageData->shaderStageCreateInfo);
        }

        return shaderStages;
    }

    void Shader::fillShaderModule(const std::unique_ptr<ShaderStageData>& shaderStageData, const std::vector<char>& shaderCode) const {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = shaderCode.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

        VkResult result = vkCreateShaderModule(GraphicService::instance()->getDevices().getLogicalDevice(), &createInfo, nullptr, &shaderStageData->shaderModule);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create shader module with error code: " + std::to_string(result));
        }

        ObjectNamingHelper::nameObject(ObjectNamingHelper::SHADER, shaderStageData->shaderModule, shaderName);
    }

    void Shader::fillPipelineShaderStage(const std::unique_ptr<ShaderStageData>& shaderStageData, ShaderType shaderType) const {
        shaderStageData->shaderStageCreateInfo = {};
        shaderStageData->shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStageData->shaderStageCreateInfo.stage = toShaderStageFlag(shaderType);
        shaderStageData->shaderStageCreateInfo.module = shaderStageData->shaderModule;
        shaderStageData->shaderStageCreateInfo.pName = "main";
        shaderStageData->shaderStageCreateInfo.pSpecializationInfo = nullptr;

        if(shaderConstants) {
            shaderStageData->specializationMapEntries.reserve(shaderConstants->getVariablesSize().size());
            uint32_t constantId = 0;
            uint32_t variableOffset = 0;
            for (auto &variableSize : shaderConstants->getVariablesSize()) {
                shaderStageData->specializationMapEntries.emplace_back(createSpecializationMapEntry(constantId++, variableOffset, variableSize));
                variableOffset += (uint32_t)variableSize;
            }

            shaderStageData->specializationInfo = {};
            shaderStageData->specializationInfo.mapEntryCount = (uint32_t) shaderStageData->specializationMapEntries.size();
            shaderStageData->specializationInfo.pMapEntries = shaderStageData->specializationMapEntries.data();
            shaderStageData->specializationInfo.dataSize = shaderConstants->sumVariablesSize();
            shaderStageData->specializationInfo.pData = shaderConstants->getData();

            shaderStageData->shaderStageCreateInfo.pSpecializationInfo = &shaderStageData->specializationInfo;
        }
    }

    VkSpecializationMapEntry Shader::createSpecializationMapEntry(uint32_t constantId, uint32_t offset, std::size_t size) {
        VkSpecializationMapEntry specializationMapEntry{};
        specializationMapEntry.constantID = constantId;
        specializationMapEntry.offset = offset;
        specializationMapEntry.size = size;
        return specializationMapEntry;
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
