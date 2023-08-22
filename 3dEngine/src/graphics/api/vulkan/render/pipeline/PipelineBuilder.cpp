#include <vulkan/generated/vk_enum_string_helper.h>

#include <graphics/api/vulkan/render/pipeline/PipelineBuilder.h>
#include <graphics/api/vulkan/render/pipeline/PipelineContainer.h>
#include <graphics/api/vulkan/helper/DebugLabelHelper.h>
#include <graphics/api/vulkan/setup/GraphicsSetupService.h>
#include <graphics/render/data/PolygonMode.h>
#include <graphics/render/data/BlendFunction.h>
#include <graphics/texture/TextureReader.h>

namespace urchin {

    PipelineBuilder::PipelineBuilder(PipelineType pipelineType, std::string name) :
            pipelineType(pipelineType),
            name(std::move(name)),
            renderTarget(nullptr),
            shader(nullptr),
            shapeType(ShapeType::TRIANGLE),
            data(nullptr),
            instanceData(nullptr),
            uniformData(nullptr),
            uniformTextureReaders(nullptr),
            uniformTextureOutputs(nullptr),
            depthTestEnabled(false),
            depthWriteEnabled(false),
            cullFaceEnabled(true),
            polygonMode(PolygonMode::FILL) {

    }

    void PipelineBuilder::setupRenderTarget(const RenderTarget& renderTarget) {
        this->renderTarget = &renderTarget;
    }

    void PipelineBuilder::setupShader(const Shader& shader) {
        this->shader = &shader;
    }

    void PipelineBuilder::setupShapeType(const ShapeType& shapeType) {
        if (pipelineType != PipelineType::GRAPHICS) {
            throw std::runtime_error("Shape type only exist on graphics pipeline");
        }
        this->shapeType = shapeType;
    }

    void PipelineBuilder::setupBlendFunctions(const std::vector<BlendFunction>& blendFunctions) {
        if (pipelineType != PipelineType::GRAPHICS) {
            throw std::runtime_error("Blend functions only exist on graphics pipeline");
        }
        this->blendFunctions = blendFunctions;
    }

    void PipelineBuilder::setupDepthOperations(bool depthTestEnabled, bool depthWriteEnabled) {
        if (pipelineType != PipelineType::GRAPHICS) {
            throw std::runtime_error("Depth operations only exist on graphics pipeline");
        }
        this->depthTestEnabled = depthTestEnabled;
        this->depthWriteEnabled = depthWriteEnabled;
    }

    void PipelineBuilder::setupCullFaceOperation(bool cullFaceEnabled) {
        if (pipelineType != PipelineType::GRAPHICS) {
            throw std::runtime_error("Cull face operation only exist on graphics pipeline");
        }
        this->cullFaceEnabled = cullFaceEnabled;
    }

    void PipelineBuilder::setupPolygonMode(PolygonMode polygonMode) {
        if (pipelineType != PipelineType::GRAPHICS) {
            throw std::runtime_error("Polygon mode only exist on graphics pipeline");
        }
        this->polygonMode = polygonMode;
    }

    void PipelineBuilder::setupData(const std::vector<DataContainer>& data, const DataContainer* instanceData) {
        if (pipelineType != PipelineType::GRAPHICS) {
            throw std::runtime_error("Data only exist on graphics pipeline");
        }
        this->data = &data;
        this->instanceData = instanceData;
    }

    void PipelineBuilder::setupUniform(const std::vector<ShaderDataContainer>& uniformData, const std::vector<std::vector<std::shared_ptr<TextureReader>>>& uniformTextureReaders,
                                       const std::vector<std::shared_ptr<Texture>>& uniformTextureOutputs) {
        this->uniformData = &uniformData;
        this->uniformTextureReaders = &uniformTextureReaders;
        this->uniformTextureOutputs = &uniformTextureOutputs;
    }

    std::shared_ptr<Pipeline> PipelineBuilder::buildPipeline() {
        checkSetup();
        std::size_t pipelineHash = computePipelineHash();

        std::shared_ptr<Pipeline> pipeline = PipelineContainer::instance().getPipeline(pipelineHash);
        if (!pipeline) {
            pipeline = std::make_shared<Pipeline>(pipelineType, pipelineHash, name);

            createDescriptorSetLayout(pipeline);
            if (pipelineType == PipelineType::GRAPHICS) {
                createGraphicsPipeline(pipeline);
            } else if (pipelineType == PipelineType::COMPUTE) {
                createComputePipeline(pipeline);
            }

            PipelineContainer::instance().addPipeline(pipeline);
        }

        return pipeline;
    }

    void PipelineBuilder::checkSetup() const {
        if (!renderTarget) {
            throw std::runtime_error("Render target not setup on pipeline");
        } else if (!shader) {
            throw std::runtime_error("Shader not setup on pipeline");
        } else if (!uniformData) {
            throw std::runtime_error("Uniform data not setup on pipeline");
        } else if (!uniformTextureReaders) {
            throw std::runtime_error("Uniform texture readers not setup on pipeline");
        } else if (!uniformTextureOutputs) {
            throw std::runtime_error("Uniform texture outputs not setup on pipeline");
        }

        if (pipelineType == PipelineType::GRAPHICS) {
            if (!data || data->empty()) {
                throw std::runtime_error("Data not setup on pipeline");
            }else if ((depthTestEnabled || depthWriteEnabled) && !renderTarget->hasDepthAttachment()) {
                throw std::runtime_error("Depth operations are enabled but there is no depth attachment on the render target");
            }
        }
    }

    std::size_t PipelineBuilder::computePipelineHash() const {
        std::size_t hash = 0;

        unsigned int repeatCount;
        if (data) {
            HashUtil::combine(hash, data->size());
            for (const DataContainer& singleData: *data) {
                HashUtil::combine(hash, getVulkanFormat(singleData, repeatCount));
                HashUtil::combine(hash, repeatCount);
            }
        }
        if (instanceData) {
            HashUtil::combine(hash, getVulkanFormat(*instanceData, repeatCount));
            HashUtil::combine(hash, repeatCount);
        }

        for (const BlendFunction& bf : blendFunctions) {
            HashUtil::combine(hash, bf.getSrcColorFactor(), bf.getDstColorFactor(), bf.getSrcAlphaFactor(), bf.getDstAlphaFactor());
        }

        HashUtil::combine(hash,
                renderTarget->getWidth(), renderTarget->getHeight(), renderTarget->getRenderPassCompatibilityId(),
                shader->getShaderId(),
                shapeType,
                depthTestEnabled, depthWriteEnabled, cullFaceEnabled, polygonMode);

        return hash;
    }

    void PipelineBuilder::createDescriptorSetLayout(const std::shared_ptr<Pipeline>& pipeline) const {
        auto logicalDevice = GraphicsSetupService::instance().getDevices().getLogicalDevice();

        uint32_t shaderUniformBinding = 0;
        std::vector<VkDescriptorSetLayoutBinding> bindings;

        VkShaderStageFlags stageFlags = 0;
        if (pipelineType == PipelineType::GRAPHICS) {
            stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        } else if (pipelineType == PipelineType::COMPUTE) {
            stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        }

        for (auto& uniformSingleData : *uniformData) {
            std::ignore = uniformSingleData;
            VkDescriptorSetLayoutBinding uboLayoutBinding{};
            uboLayoutBinding.binding = shaderUniformBinding++;
            uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            uboLayoutBinding.descriptorCount = 1;
            uboLayoutBinding.stageFlags = stageFlags;
            uboLayoutBinding.pImmutableSamplers = nullptr;
            bindings.emplace_back(uboLayoutBinding);
        }

        for (auto& uniformTextureReader : *uniformTextureReaders) {
            VkDescriptorSetLayoutBinding samplerLayoutBinding{};
            samplerLayoutBinding.binding = shaderUniformBinding++;
            samplerLayoutBinding.descriptorCount = (uint32_t)uniformTextureReader.size();
            samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            samplerLayoutBinding.pImmutableSamplers = nullptr;
            samplerLayoutBinding.stageFlags = stageFlags;
            bindings.emplace_back(samplerLayoutBinding);
        }

        for (std::size_t i = 0; i < uniformTextureOutputs->size(); ++i) {
            VkDescriptorSetLayoutBinding samplerLayoutBinding{};
            samplerLayoutBinding.binding = shaderUniformBinding++;
            samplerLayoutBinding.descriptorCount = (uint32_t)1;
            samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            samplerLayoutBinding.pImmutableSamplers = nullptr;
            samplerLayoutBinding.stageFlags = stageFlags;
            bindings.emplace_back(samplerLayoutBinding);
        }

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = (uint32_t)bindings.size();
        layoutInfo.pBindings = bindings.empty() ? nullptr : bindings.data();

        VkResult result = vkCreateDescriptorSetLayout(logicalDevice, &layoutInfo, nullptr, &pipeline->descriptorSetLayout());
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor set layout with error code: " + std::string(string_VkResult(result)));
        }
    }

    void PipelineBuilder::createGraphicsPipeline(const std::shared_ptr<Pipeline>& pipeline) {
        auto logicalDevice = GraphicsSetupService::instance().getDevices().getLogicalDevice();
        auto shaderStages = shader->getShaderStages();

        //vertex input stage
        std::vector<VkVertexInputBindingDescription> bindingDescriptions;
        bindingDescriptions.reserve(data->size() + ((instanceData != nullptr) ? 1 : 0));
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
        attributeDescriptions.reserve(data->size()); //estimated memory size

        uint32_t binding = 0;
        uint32_t shaderLocation = 0;
        for (const auto& singleData : *data) {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = binding; //binding for vkCmdBindVertexBuffers(..., firstBinding, ...)
            bindingDescription.stride = (uint32_t)singleData.getDataSize();
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            bindingDescriptions.emplace_back(bindingDescription);

            unsigned int repeatCount = 0;
            VkFormat attributeFormat = getVulkanFormat(singleData, repeatCount);
            for (unsigned int i = 0; i < repeatCount; ++i) {
                VkVertexInputAttributeDescription attributeDescription{};
                attributeDescription.binding = binding;
                attributeDescription.location = shaderLocation;
                attributeDescription.format = attributeFormat;
                attributeDescription.offset = (uint32_t)(i * (singleData.getDataSize() / repeatCount));
                attributeDescriptions.emplace_back(attributeDescription);
                shaderLocation++;
            }
            binding++;
        }

        if (instanceData) {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = binding; //binding for vkCmdBindVertexBuffers(..., firstBinding, ...)
            bindingDescription.stride = (uint32_t)instanceData->getDataSize();
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
            bindingDescriptions.emplace_back(bindingDescription);

            unsigned int repeatCount = 0;
            VkFormat attributeFormat = getVulkanFormat(*instanceData, repeatCount);
            for (unsigned int i = 0; i < repeatCount; ++i) {
                VkVertexInputAttributeDescription attributeDescription{};
                attributeDescription.binding = binding;
                attributeDescription.location = shaderLocation;
                attributeDescription.format = attributeFormat;
                attributeDescription.offset = (uint32_t)(i * (instanceData->getDataSize() / repeatCount));
                attributeDescriptions.emplace_back(attributeDescription);
                shaderLocation++;
            }
        }

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = (uint32_t)bindingDescriptions.size();
        vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.empty() ? nullptr : bindingDescriptions.data();
        vertexInputInfo.vertexAttributeDescriptionCount = (uint32_t)attributeDescriptions.size();
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.empty() ? nullptr : attributeDescriptions.data();

        //input assembly stage
        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = shapeTypeToVulkanTopology();
        inputAssembly.primitiveRestartEnable = isShapeTypeListTopology() ? VK_FALSE : VK_TRUE;

        //viewports and scissors stage
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)renderTarget->getWidth();
        viewport.height = (float)renderTarget->getHeight();
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor;
        scissor.offset = {.x = 0, .y = 0};
        scissor.extent = {.width = renderTarget->getWidth(), .height = renderTarget->getHeight()};
        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        //rasterization stage (vertices turned into fragments + depth testing + face culling)
        VkPipelineRasterizationStateCreateInfo rasterization{};
        rasterization.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterization.rasterizerDiscardEnable = VK_FALSE;
        rasterization.polygonMode = (polygonMode == PolygonMode::WIREFRAME) ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
        rasterization.lineWidth = 1.0f; //do not use a value != 1.0f: less than 60% of graphic cards support it
        rasterization.cullMode = cullFaceEnabled ? VK_CULL_MODE_BACK_BIT : VK_CULL_MODE_NONE;
        rasterization.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterization.depthClampEnable = VK_FALSE;
        rasterization.depthBiasEnable = VK_FALSE;
        rasterization.depthBiasConstantFactor = 0.0f;
        rasterization.depthBiasClamp = 0.0f;
        rasterization.depthBiasSlopeFactor = 0.0f;

        //multisampling stage (mainly for anti-aliasing)
        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f;
        multisampling.pSampleMask = nullptr;
        multisampling.alphaToCoverageEnable = VK_FALSE;
        multisampling.alphaToOneEnable = VK_FALSE;

        //depth and stencil stage
        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = depthTestEnabled ? VK_TRUE : VK_FALSE;
        depthStencil.depthWriteEnable = depthWriteEnabled ? VK_TRUE : VK_FALSE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.minDepthBounds = 0.0f;
        depthStencil.maxDepthBounds = 1.0f;
        depthStencil.stencilTestEnable = VK_FALSE;
        depthStencil.front = {};
        depthStencil.back = {};

        //color blending
        if (!blendFunctions.empty() && blendFunctions.size() != renderTarget->getNumColorAttachment()) {
            throw std::runtime_error("Number of blend functions (" + std::to_string(blendFunctions.size()) + ") does not match with number of color attachments (" + std::to_string(renderTarget->getNumColorAttachment()) + ")");
        } else {
            blendFunctions.resize(renderTarget->getNumColorAttachment(), BlendFunction::buildBlendDisabled());
        }
        std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
        for (std::size_t i = 0; i < renderTarget->getNumColorAttachment(); ++i) {
            VkPipelineColorBlendAttachmentState colorBlendAttachment{};
            colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            colorBlendAttachment.blendEnable = blendFunctions[i].isBlendEnabled() ? VK_TRUE : VK_FALSE;

            colorBlendAttachment.srcColorBlendFactor = toVkBlenderFactor(blendFunctions[i].getSrcColorFactor());
            colorBlendAttachment.dstColorBlendFactor = toVkBlenderFactor(blendFunctions[i].getDstColorFactor());
            colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;

            colorBlendAttachment.srcAlphaBlendFactor = toVkBlenderFactor(blendFunctions[i].getSrcAlphaFactor());
            colorBlendAttachment.dstAlphaBlendFactor = toVkBlenderFactor(blendFunctions[i].getDstAlphaFactor());
            colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

            colorBlendAttachments.push_back(colorBlendAttachment);
        }
        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = (uint32_t)colorBlendAttachments.size();
        colorBlending.pAttachments = colorBlendAttachments.empty() ? nullptr : colorBlendAttachments.data();
        colorBlending.blendConstants[0] = 0.0f; colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f; colorBlending.blendConstants[3] = 0.0f;

        //dynamic state
        std::vector<VkDynamicState> dynamicsStates = {VK_DYNAMIC_STATE_SCISSOR};
        VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
        pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        pipelineDynamicStateCreateInfo.dynamicStateCount = (uint32_t)dynamicsStates.size();
        pipelineDynamicStateCreateInfo.pDynamicStates = dynamicsStates.data();

        //pipeline layout (used to transfer uniform to shaders)
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &pipeline->getDescriptorSetLayout();
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
        VkResult pipelineLayoutResult = vkCreatePipelineLayout(logicalDevice, &pipelineLayoutInfo, nullptr, &pipeline->pipelineLayout());
        if (pipelineLayoutResult != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout with error code: " + std::string(string_VkResult(pipelineLayoutResult)));
        }

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = (uint32_t)shaderStages.size();
        pipelineInfo.pStages = shaderStages.data();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterization;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = &depthStencil;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &pipelineDynamicStateCreateInfo;
        pipelineInfo.layout = pipeline->getPipelineLayout();
        pipelineInfo.renderPass = renderTarget->getRenderPass();
        pipelineInfo.subpass = 0; //index to sub-pass
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; //can be used to switch optimally between similar pipeline
        pipelineInfo.basePipelineIndex = -1;

        VkResult pipelinesResult = vkCreateGraphicsPipelines(logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline->vkPipeline());
        if (pipelinesResult != VK_SUCCESS) {
            throw std::runtime_error("Failed to create graphics pipeline with error code: " + std::string(string_VkResult(pipelinesResult)));
        }

        DebugLabelHelper::nameObject(DebugLabelHelper::PIPELINE, pipeline->vkPipeline(), name);
    }

    VkPrimitiveTopology PipelineBuilder::shapeTypeToVulkanTopology() const {
        if (shapeType == ShapeType::TRIANGLE) {
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        } else if (shapeType == ShapeType::TRIANGLE_STRIP) {
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        } else if (shapeType == ShapeType::POINT) {
            return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        }
        throw std::runtime_error("Unknown shape type: " + std::to_string((int)shapeType));
    }

    bool PipelineBuilder::isShapeTypeListTopology() const {
        if (shapeType == ShapeType::TRIANGLE || shapeType == ShapeType::POINT) {
            return true;
        } else if (shapeType == ShapeType::TRIANGLE_STRIP) {
            return false;
        }
        throw std::runtime_error("Unknown shape type: " + std::to_string((int)shapeType));
    }

    /**
     * @param repeatCount [out] Number of data repetition
     */
    VkFormat PipelineBuilder::getVulkanFormat(const DataContainer& dataContainer, unsigned int& repeatCount) const {
        if (dataContainer.getDataType() == DataType::FLOAT) {
            if (dataContainer.getVariableType() == VariableType::FLOAT) {
                repeatCount = 1;
                return VK_FORMAT_R32_SFLOAT;
            } else if (dataContainer.getVariableType() == VariableType::VEC2) {
                repeatCount = 1;
                return VK_FORMAT_R32G32_SFLOAT;
            } else if (dataContainer.getVariableType() == VariableType::VEC3) {
                repeatCount = 1;
                return VK_FORMAT_R32G32B32_SFLOAT;
            } else if (dataContainer.getVariableType() == VariableType::MAT4) {
                repeatCount = 4;
                return VK_FORMAT_R32G32B32A32_SFLOAT;
            } else if (dataContainer.getVariableType() == VariableType::TWO_MAT4) {
                repeatCount = 8;
                return VK_FORMAT_R32G32B32A32_SFLOAT;
            }
            throw std::runtime_error("Unknown variable type: " + std::to_string((int)dataContainer.getVariableType()));
        }
        throw std::runtime_error("Unknown data type: " + std::to_string((int)dataContainer.getDataType()));
    }

    VkBlendFactor PipelineBuilder::toVkBlenderFactor(BlendFactor blendFactor) const {
        if (blendFactor == BlendFactor::SRC_ALPHA) {
            return VK_BLEND_FACTOR_SRC_ALPHA;
        } else if (blendFactor == BlendFactor::ONE_MINUS_SRC_ALPHA) {
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        } else if (blendFactor == BlendFactor::ONE_MINUS_SRC_COLOR) {
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        } else if (blendFactor == BlendFactor::ONE) {
            return VK_BLEND_FACTOR_ONE;
        } else if (blendFactor == BlendFactor::ZERO) {
            return VK_BLEND_FACTOR_ZERO;
        }
        throw std::runtime_error("Unknown blend factor: " + std::to_string((int)blendFactor));
    }

    void PipelineBuilder::createComputePipeline(const std::shared_ptr<Pipeline>& pipeline) const {
        auto logicalDevice = GraphicsSetupService::instance().getDevices().getLogicalDevice();

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &pipeline->descriptorSetLayout();

        VkResult pipelineLayoutResult = vkCreatePipelineLayout(logicalDevice, &pipelineLayoutInfo, nullptr, &pipeline->pipelineLayout());
        if (pipelineLayoutResult != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout with error code: " + std::string(string_VkResult(pipelineLayoutResult)));
        }

        VkComputePipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        pipelineInfo.layout = pipeline->getPipelineLayout();
        pipelineInfo.stage = shader->getShaderStages()[0];

        VkResult pipelinesResult = vkCreateComputePipelines(logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline->vkPipeline());
        if (pipelinesResult != VK_SUCCESS) {
            throw std::runtime_error("Failed to create compute pipeline with error code: " + std::string(string_VkResult(pipelinesResult)));
        }

        DebugLabelHelper::nameObject(DebugLabelHelper::PIPELINE, pipeline->vkPipeline(), name);
    }

}
