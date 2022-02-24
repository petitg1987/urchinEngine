#include <api/render/pipeline/PipelineBuilder.h>
#include <api/render/pipeline/PipelineContainer.h>
#include <api/render/model/PolygonMode.h>
#include <api/render/blend/BlendFunction.h>
#include <api/helper/DebugLabelHelper.h>
#include <api/setup/GraphicService.h>

namespace urchin {

    PipelineBuilder::PipelineBuilder(std::string name) :
            name(std::move(name)),
            renderTarget(nullptr),
            shader(nullptr),
            shapeType(ShapeType::TRIANGLE),
            data(nullptr),
            instanceData(nullptr),
            uniformData(nullptr),
            uniformTextureReaders(nullptr),
            depthTestEnabled(false),
            depthWriteEnabled(false),
            cullFaceEnabled(true),
            polygonMode(PolygonMode::FILL),
            scissorEnabled(false) {

    }

    void PipelineBuilder::setupRenderTarget(const RenderTarget& renderTarget) {
        this->renderTarget = &renderTarget;
    }

    void PipelineBuilder::setupShader(const Shader& shader) {
        this->shader = &shader;
    }

    void PipelineBuilder::setupShapeType(const ShapeType& shapeType) {
        this->shapeType = shapeType;
    }

    void PipelineBuilder::setupBlendFunctions(const std::vector<BlendFunction>& blendFunctions) {
        this->blendFunctions = blendFunctions;
    }

    void PipelineBuilder::setupDepthOperations(bool depthTestEnabled, bool depthWriteEnabled) {
        this->depthTestEnabled = depthTestEnabled;
        this->depthWriteEnabled = depthWriteEnabled;
    }

    void PipelineBuilder::setupCallFaceOperation(bool cullFaceEnabled) {
        this->cullFaceEnabled = cullFaceEnabled;
    }

    void PipelineBuilder::setupPolygonMode(PolygonMode polygonMode) {
        this->polygonMode = polygonMode;
    }

    void PipelineBuilder::setupScissor(bool scissorEnabled, Vector2<int> scissorOffset, Vector2<int> scissorSize) {
        this->scissorEnabled = scissorEnabled;
        this->scissorOffset = scissorOffset;
        this->scissorSize = scissorSize;
    }

    void PipelineBuilder::setupData(const std::vector<DataContainer>& data, const DataContainer* instanceData) {
        this->data = &data;
        this->instanceData = instanceData;
    }

    void PipelineBuilder::setupUniform(const std::vector<ShaderDataContainer>& uniformData, const std::vector<std::vector<std::shared_ptr<TextureReader>>>& uniformTextureReaders) {
        this->uniformData = &uniformData;
        this->uniformTextureReaders = &uniformTextureReaders;
    }

    std::shared_ptr<Pipeline> PipelineBuilder::buildPipeline() {
        checkSetup();
        std::size_t pipelineHash = computePipelineHash();

        std::shared_ptr<Pipeline> pipeline = PipelineContainer::instance().getPipeline(pipelineHash);
        if (!pipeline) {
            pipeline = std::make_shared<Pipeline>(pipelineHash, name);

            createDescriptorSetLayout(pipeline);
            createGraphicsPipeline(pipeline);

            PipelineContainer::instance().addPipeline(pipeline);
        }

        return pipeline;
    }

    void PipelineBuilder::checkSetup() const {
        if (!renderTarget) {
            throw std::runtime_error("Render target not setup on pipeline");
        } else if (!shader) {
            throw std::runtime_error("Shader not setup on pipeline");
        } else if (!data || data->empty()) {
            throw std::runtime_error("Data not setup on pipeline");
        } else if (!uniformData) {
            throw std::runtime_error("Uniform data not setup on pipeline");
        } else if (!uniformTextureReaders) {
            throw std::runtime_error("Uniform texture readers not setup on pipeline");
        }

        if ((depthTestEnabled || depthWriteEnabled) && !renderTarget->hasDepthAttachment()) {
            throw std::runtime_error("Depth operations are enabled but there is no depth attachment on the render target");
        }
    }

    std::size_t PipelineBuilder::computePipelineHash() const {
        std::size_t hash = 0;

        unsigned int repeatCount;
        HashUtil::combine(hash, data->size());
        for (auto& singleData : *data) {
            HashUtil::combine(hash, singleData.getVulkanFormat(repeatCount));
            HashUtil::combine(hash, repeatCount);
        }
        if (instanceData) {
            HashUtil::combine(hash, instanceData->getVulkanFormat(repeatCount));
            HashUtil::combine(hash, repeatCount);
        }

        for (auto& bf : blendFunctions) {
            HashUtil::combine(hash, bf.getSrcColorFactor(), bf.getDstColorFactor(), bf.getSrcAlphaFactor(), bf.getDstAlphaFactor());
        }

        if (scissorEnabled) {
            HashUtil::combine(hash, scissorOffset.X, scissorOffset.Y, scissorSize.X, scissorSize.Y);
        }

        HashUtil::combine(hash,
                renderTarget->getWidth(), renderTarget->getHeight(), renderTarget->getRenderPassCompatibilityId(),
                shader->getShaderId(),
                shapeType,
                depthTestEnabled, depthWriteEnabled, cullFaceEnabled, polygonMode);

        return hash;
    }

    void PipelineBuilder::createDescriptorSetLayout(const std::shared_ptr<Pipeline>& pipeline) const {
        auto logicalDevice = GraphicService::instance().getDevices().getLogicalDevice();

        uint32_t shaderUniformBinding = 0;
        std::vector<VkDescriptorSetLayoutBinding> bindings;

        for (auto& uniformSingleData : *uniformData) {
            std::ignore = uniformSingleData;
            VkDescriptorSetLayoutBinding uboLayoutBinding{};
            uboLayoutBinding.binding = shaderUniformBinding++;
            uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            uboLayoutBinding.descriptorCount = 1;
            uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
            uboLayoutBinding.pImmutableSamplers = nullptr;
            bindings.emplace_back(uboLayoutBinding);
        }

        for (auto& uniformTextureReader : *uniformTextureReaders) {
            VkDescriptorSetLayoutBinding samplerLayoutBinding{};
            samplerLayoutBinding.binding = shaderUniformBinding++;
            samplerLayoutBinding.descriptorCount = (uint32_t)uniformTextureReader.size();
            samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            samplerLayoutBinding.pImmutableSamplers = nullptr;
            samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
            bindings.emplace_back(samplerLayoutBinding);
        }

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = (uint32_t)bindings.size();
        layoutInfo.pBindings = bindings.data();

        VkResult result = vkCreateDescriptorSetLayout(logicalDevice, &layoutInfo, nullptr, &pipeline->descriptorSetLayout());
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor set layout with error code: " + std::to_string(result));
        }
    }

    void PipelineBuilder::createGraphicsPipeline(const std::shared_ptr<Pipeline>& pipeline) {
        auto logicalDevice = GraphicService::instance().getDevices().getLogicalDevice();
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
            VkFormat attributeFormat = singleData.getVulkanFormat(repeatCount);
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
            VkFormat attributeFormat = instanceData->getVulkanFormat(repeatCount);
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
        vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
        vertexInputInfo.vertexAttributeDescriptionCount = (uint32_t)attributeDescriptions.size();
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

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
        VkRect2D scissor = {};
        if (scissorEnabled) {
            scissor.offset = {scissorOffset.X, scissorOffset.Y};
            scissor.extent = {(uint32_t)scissorSize.X, (uint32_t)scissorSize.Y};
        } else {
            scissor.offset = {0, 0};
            scissor.extent = {renderTarget->getWidth(), renderTarget->getHeight()};
        }
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
            blendFunctions[i].setupColorBlend(colorBlendAttachment);
            colorBlendAttachments.push_back(colorBlendAttachment);
        }
        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = (uint32_t)colorBlendAttachments.size();
        colorBlending.pAttachments = colorBlendAttachments.data();
        colorBlending.blendConstants[0] = 0.0f; colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f; colorBlending.blendConstants[3] = 0.0f;

        //pipeline layout (used to transfer uniform to shaders)
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &pipeline->getDescriptorSetLayout();
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
        VkResult pipelineLayoutResult = vkCreatePipelineLayout(logicalDevice, &pipelineLayoutInfo, nullptr, &pipeline->pipelineLayout());
        if (pipelineLayoutResult != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout with error code: " + std::to_string(pipelineLayoutResult));
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
        pipelineInfo.pDynamicState = nullptr;
        pipelineInfo.layout = pipeline->getPipelineLayout();
        pipelineInfo.renderPass = renderTarget->getRenderPass();
        pipelineInfo.subpass = 0; //index to sub-pass
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; //can be used to switch optimally between similar pipeline
        pipelineInfo.basePipelineIndex = -1;

        VkResult pipelinesResult = vkCreateGraphicsPipelines(logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline->graphicsPipeline());
        if (pipelinesResult != VK_SUCCESS) {
            throw std::runtime_error("Failed to create graphics pipeline with error code: " + std::to_string(pipelinesResult));
        }

        DebugLabelHelper::nameObject(DebugLabelHelper::PIPELINE, pipeline->graphicsPipeline(), name);
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

}
