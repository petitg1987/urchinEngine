#include "GenericRenderer.h"
#include "graphic/setup/GraphicService.h"
#include "graphic/render/GenericRendererBuilder.h"
#include "graphic/render/model/DataContainer.h"

namespace urchin {

    //static
    const uint32_t GenericRenderer::PRIMITIVE_RESTART_INDEX_VALUE = 0xFFFFFFFF;
    const uint32_t GenericRenderer::UNIFORM_TEX_BINDING_START_INDEX = 20;

    GenericRenderer::GenericRenderer(const GenericRendererBuilder* rendererBuilder) :
            isInitialized(false),
            renderTarget(rendererBuilder->getRenderTarget()),
            shader(rendererBuilder->getShader()),
            shapeType(rendererBuilder->getShapeType()),
            data(rendererBuilder->getData()),
            indices(rendererBuilder->getIndices()),
            shaderData(rendererBuilder->getShaderData()),
            textureReaders(rendererBuilder->getTextureReaders()),
            transparencyEnabled(rendererBuilder->isTransparencyEnabled()),
            depthOperationsEnabled(rendererBuilder->isDepthOperationsEnabled()),
            cullFaceEnabled(rendererBuilder->isCullFaceEnabled()),
            polygonMode(rendererBuilder->getPolygonMode()),
            outlineSize(rendererBuilder->getOutlineSize()),
            descriptorSetLayout(nullptr),
            pipelineLayout(nullptr),
            graphicsPipeline(nullptr),
            descriptorPool(nullptr),
            drawCommandDirty(false) {
        if(depthOperationsEnabled && !renderTarget->hasDepthAttachment()) {
            throw std::runtime_error("Depth operations are enabled but there is no depth attachment on the render target");
        }
        if(shaderData.size() > UNIFORM_TEX_BINDING_START_INDEX) {
            throw std::runtime_error("Too much of shader data (" + std::to_string(shaderData.size()) + "), limit is: " + std::to_string(UNIFORM_TEX_BINDING_START_INDEX));
        }

        for(auto& textureReaderArray : textureReaders) {
            for(auto& textureReader : textureReaderArray) {
                textureReader->initialize();
            }
        }

        initialize();
        renderTarget->addRenderer(this);
    }

    GenericRenderer::~GenericRenderer() {
        renderTarget->removeRenderer(this);

        cleanup();
        textureReaders.clear();
    }

    void GenericRenderer::initialize() {
        assert(!isInitialized);

        createDescriptorSetLayout();
        createGraphicsPipeline();
        createVertexBuffers();
        createIndexBuffer();
        createUniformBuffers();
        createDescriptorPool();
        createDescriptorSets();

        isInitialized = true;
        drawCommandDirty = true;
    }

    void GenericRenderer::cleanup() {
        if (isInitialized) {
            vkDeviceWaitIdle(GraphicService::instance()->getDevices().getLogicalDevice());

            destroyDescriptorSetsAndPool();
            destroyUniformBuffers();
            destroyIndexBuffer();
            destroyVertexBuffers();
            destroyGraphicsPipeline();
            destroyDescriptorSetLayout();

            isInitialized = false;
        }
    }

    const std::shared_ptr<RenderTarget> &GenericRenderer::getRenderTarget() const {
        return renderTarget;
    }

    bool GenericRenderer::isDrawCommandDirty() const {
        return drawCommandDirty;
    }

    void GenericRenderer::createDescriptorSetLayout() {
        auto logicalDevice = GraphicService::instance()->getDevices().getLogicalDevice();

        std::vector<VkDescriptorSetLayoutBinding> bindings;

        for(unsigned int i = 0; i < shaderData.size(); ++i) {
            VkDescriptorSetLayoutBinding uboLayoutBinding{};
            uboLayoutBinding.binding = i; //shader uniform binding
            uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            uboLayoutBinding.descriptorCount = 1;
            uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
            uboLayoutBinding.pImmutableSamplers = nullptr;
            bindings.emplace_back(uboLayoutBinding);
        }

        for(unsigned int i = 0; i < textureReaders.size(); ++i) {
            VkDescriptorSetLayoutBinding samplerLayoutBinding{};
            samplerLayoutBinding.binding = UNIFORM_TEX_BINDING_START_INDEX + i; //shader uniform binding
            samplerLayoutBinding.descriptorCount = (uint32_t)textureReaders[i].size();
            samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            samplerLayoutBinding.pImmutableSamplers = nullptr;
            samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
            bindings.emplace_back(samplerLayoutBinding);
        }

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = (uint32_t)bindings.size();
        layoutInfo.pBindings = bindings.data();

        VkResult result = vkCreateDescriptorSetLayout(logicalDevice, &layoutInfo, nullptr, &descriptorSetLayout);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor set layout with error code: " + std::to_string(result));
        }
    }

    void GenericRenderer::destroyDescriptorSetLayout() {
        vkDestroyDescriptorSetLayout(GraphicService::instance()->getDevices().getLogicalDevice(), descriptorSetLayout, nullptr);
    }

    void GenericRenderer::createGraphicsPipeline() {
        auto logicalDevice = GraphicService::instance()->getDevices().getLogicalDevice();
        auto shaderStages = shader->getShaderStages();

        //vertex input stage
        std::vector<VkVertexInputBindingDescription> bindingDescriptions;
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
        for(uint32_t i = 0; i < data.size(); ++i) {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = i;
            bindingDescription.stride = (uint32_t)data[i].getDataSize();
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            bindingDescriptions.emplace_back(bindingDescription);

            VkVertexInputAttributeDescription attributeDescription{};
            attributeDescription.binding = i;
            attributeDescription.location = i;
            attributeDescription.format = data[i].getVulkanFormat();
            attributeDescription.offset = 0;
            attributeDescriptions.emplace_back(attributeDescription);
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
        inputAssembly.topology = shapeTypeToVulkanType();
        inputAssembly.primitiveRestartEnable = (indices && indices->hasPrimitiveRestartIndex()) ? VK_TRUE : VK_FALSE;

        //viewports and scissors stage
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)renderTarget->getWidth();
        viewport.height = (float)renderTarget->getHeight();
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor = {{0, 0}, {renderTarget->getWidth(), renderTarget->getHeight()}};
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
        rasterization.polygonMode = polygonMode == PolygonMode::WIREFRAME ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
        rasterization.lineWidth = outlineSize;
        rasterization.cullMode = cullFaceEnabled ? VK_CULL_MODE_BACK_BIT : VK_CULL_MODE_NONE;
        rasterization.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterization.depthClampEnable = VK_FALSE;
        rasterization.depthBiasEnable = VK_FALSE;
        rasterization.depthBiasConstantFactor = 0.0f;
        rasterization.depthBiasClamp = 0.0f;
        rasterization.depthBiasSlopeFactor = 0.0f;

        //multisampling stage (manly for anti-aliasing)
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
        depthStencil.depthTestEnable = depthOperationsEnabled ? VK_TRUE : VK_FALSE;
        depthStencil.depthWriteEnable = depthOperationsEnabled ? VK_TRUE : VK_FALSE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.minDepthBounds = 0.0f;
        depthStencil.maxDepthBounds = 1.0f;
        depthStencil.stencilTestEnable = VK_FALSE;
        depthStencil.front = {};
        depthStencil.back = {};

        //color blending
        std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
        for(std::size_t i = 0; i < renderTarget->getNumColorAttachment(); ++i) {
            VkPipelineColorBlendAttachmentState colorBlendAttachment{};
            colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            colorBlendAttachment.blendEnable = transparencyEnabled ? VK_TRUE : VK_FALSE;
            colorBlendAttachment.srcColorBlendFactor = transparencyEnabled ? VK_BLEND_FACTOR_SRC_ALPHA : VK_BLEND_FACTOR_ONE;
            colorBlendAttachment.dstColorBlendFactor = transparencyEnabled ? VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA : VK_BLEND_FACTOR_ZERO;
            colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
            colorBlendAttachment.srcAlphaBlendFactor = transparencyEnabled ? VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA : VK_BLEND_FACTOR_ONE;
            colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
            colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
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
        pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
        VkResult pipelineLayoutResult = vkCreatePipelineLayout(logicalDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout);
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
        pipelineInfo.layout = pipelineLayout;
        pipelineInfo.renderPass = renderTarget->getRenderPass();
        pipelineInfo.subpass = 0; //index to sub-pass
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; //can be used to switch optimally between similar pipeline
        pipelineInfo.basePipelineIndex = -1;

        VkResult pipelinesResult = vkCreateGraphicsPipelines(logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline);
        if (pipelinesResult != VK_SUCCESS) {
            throw std::runtime_error("Failed to create graphics pipeline with error code: " + std::to_string(pipelinesResult));
        }
    }

    void GenericRenderer::destroyGraphicsPipeline() {
        auto logicalDevice = GraphicService::instance()->getDevices().getLogicalDevice();

        vkDestroyPipeline(logicalDevice, graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(logicalDevice, pipelineLayout, nullptr);
    }

    VkPrimitiveTopology GenericRenderer::shapeTypeToVulkanType() const {
        if (shapeType == ShapeType::TRIANGLE) {
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        } else if (shapeType == ShapeType::TRIANGLE_STRIP) {
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        } else if (shapeType == ShapeType::LINE) {
            return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        } else if (shapeType == ShapeType::LINE_STRIP) {
            return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
        } else if (shapeType == ShapeType::POINT) {
            return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        }
        throw std::runtime_error("Unknown shape type: " + std::to_string(shapeType));
    }

    void GenericRenderer::createVertexBuffers() {
        vertexBuffers.resize(data.size());
        for (std::size_t dataIndex = 0; dataIndex < data.size(); ++dataIndex) {
            const DataContainer& dataContainer = data[dataIndex];
            vertexBuffers[dataIndex].initialize(BufferHandler::VERTEX, renderTarget->getNumFramebuffer(), dataContainer.getBufferSize(), dataContainer.getData());
        }
    }

    void GenericRenderer::destroyVertexBuffers() {
        for(auto& vertexBuffer : vertexBuffers) {
            vertexBuffer.cleanup();
        }
        vertexBuffers.clear();
    }

    void GenericRenderer::createIndexBuffer() {
        if (indices) {
            indexBuffer.initialize(BufferHandler::INDEX, renderTarget->getNumFramebuffer(), indices->getBufferSize(), indices->getIndices());
        }
    }

    void GenericRenderer::destroyIndexBuffer() {
        if (indices) {
            indexBuffer.cleanup();
        }
    }

    void GenericRenderer::createUniformBuffers() {
        uniformsBuffers.resize(shaderData.size());
        for (std::size_t dataIndex = 0; dataIndex < shaderData.size(); ++dataIndex) {
            uniformsBuffers[dataIndex].initialize(BufferHandler::UNIFORM, renderTarget->getNumFramebuffer(), shaderData[dataIndex].getDataSize());
        }
    }

    void GenericRenderer::destroyUniformBuffers() {
        for (auto& uniformsBuffer : uniformsBuffers) {
            uniformsBuffer.cleanup();
        }
        uniformsBuffers.clear();
    }

    void GenericRenderer::createDescriptorPool() {
        auto logicalDevice = GraphicService::instance()->getDevices().getLogicalDevice();

        std::array<VkDescriptorPoolSize, 2> poolSizes{};
        int uboDescriptorCount = std::max(1, (int)renderTarget->getNumFramebuffer() * (int)shaderData.size());
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = (uint32_t)uboDescriptorCount;
        int textureDescriptorCount = std::max(1, (int)renderTarget->getNumFramebuffer() * (int)textureReaders.size());
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = (uint32_t)textureDescriptorCount;

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = (uint32_t)poolSizes.size();
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = (uint32_t)renderTarget->getNumFramebuffer();

        VkResult result = vkCreateDescriptorPool(logicalDevice, &poolInfo, nullptr, &descriptorPool);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor pool with error code: " + std::to_string(result));
        }
    }

    void GenericRenderer::createDescriptorSets() {
        auto logicalDevice = GraphicService::instance()->getDevices().getLogicalDevice();

        std::vector<VkDescriptorSetLayout> layouts(renderTarget->getNumFramebuffer(), descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = (uint32_t)renderTarget->getNumFramebuffer();
        allocInfo.pSetLayouts = layouts.data();

        descriptorSets.resize(renderTarget->getNumFramebuffer());
        VkResult result =  vkAllocateDescriptorSets(logicalDevice, &allocInfo, descriptorSets.data());
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate descriptor sets with error code: " + std::to_string(result));
        }

        updateDescriptorSets();
    }

    void GenericRenderer::updateDescriptorSets() {
        auto logicalDevice = GraphicService::instance()->getDevices().getLogicalDevice();

        for (std::size_t frameIndex = 0; frameIndex < renderTarget->getNumFramebuffer(); frameIndex++) {
            std::vector<VkWriteDescriptorSet> descriptorWrites;

            //uniform buffer objects
            uint32_t shaderUniformBinding = 0;
            std::vector<VkDescriptorBufferInfo> bufferInfos;
            for(std::size_t dataIndex = 0; dataIndex < shaderData.size(); ++dataIndex) {
                VkDescriptorBufferInfo bufferInfo{};
                bufferInfo.buffer = uniformsBuffers[dataIndex].getBuffer(frameIndex);
                bufferInfo.offset = 0;
                bufferInfo.range = VK_WHOLE_SIZE;
                bufferInfos.emplace_back(bufferInfo);
            }
            for(auto& bufferInfo : bufferInfos) {
                VkWriteDescriptorSet uniformDescriptorWrites{};
                uniformDescriptorWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                uniformDescriptorWrites.dstSet = descriptorSets[frameIndex];
                uniformDescriptorWrites.dstBinding = shaderUniformBinding++;
                uniformDescriptorWrites.dstArrayElement = 0;
                uniformDescriptorWrites.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                uniformDescriptorWrites.descriptorCount = 1;
                uniformDescriptorWrites.pBufferInfo = &bufferInfo; //warning: bufferInfo cannot be destroyed before calling vkUpdateDescriptorSets
                descriptorWrites.emplace_back(uniformDescriptorWrites);
            }

            //textures
            shaderUniformBinding = UNIFORM_TEX_BINDING_START_INDEX;
            std::vector<std::vector<VkDescriptorImageInfo>> imageInfos;
            for(auto& textureReaderArray : textureReaders) {
                std::vector<VkDescriptorImageInfo> imageInfosArray;
                for(auto& textureReader : textureReaderArray) {
                    VkDescriptorImageInfo imageInfo{};
                    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    imageInfo.imageView = textureReader->getTexture()->getImageView();
                    imageInfo.sampler = textureReader->getParam().getTextureSampler();
                    imageInfosArray.emplace_back(imageInfo);
                }
                imageInfos.emplace_back(imageInfosArray);
            }
            for(auto& imageInfo : imageInfos) {
                VkWriteDescriptorSet textureDescriptorWrites{};
                textureDescriptorWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                textureDescriptorWrites.dstSet = descriptorSets[frameIndex];
                textureDescriptorWrites.dstBinding = shaderUniformBinding++;
                textureDescriptorWrites.dstArrayElement = 0;
                textureDescriptorWrites.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                textureDescriptorWrites.descriptorCount = (uint32_t)imageInfo.size();
                textureDescriptorWrites.pImageInfo = imageInfo.data(); //warning: imageInfo cannot be destroyed before calling vkUpdateDescriptorSets
                descriptorWrites.emplace_back(textureDescriptorWrites);
            }

            vkUpdateDescriptorSets(logicalDevice, (uint32_t)(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }

        drawCommandDirty = true;
    }

    void GenericRenderer::destroyDescriptorSetsAndPool() {
        vkDestroyDescriptorPool(GraphicService::instance()->getDevices().getLogicalDevice(), descriptorPool, nullptr);
    }

    void GenericRenderer::updateData(std::size_t dataIndex, const std::vector<Point2<float>>& dataPtr) {
        DataContainer dataContainer(DataType::FLOAT, DataDimension::TWO_DIMENSION, dataPtr.size(), dataPtr.data());
        updateData(dataIndex, std::move(dataContainer));
    }

    void GenericRenderer::updateData(std::size_t dataIndex, const std::vector<Point3<float>>& dataPtr) {
        DataContainer dataContainer(DataType::FLOAT, DataDimension::THREE_DIMENSION, dataPtr.size(), dataPtr.data());
        updateData(dataIndex, std::move(dataContainer));
    }

    void GenericRenderer::updateData(std::size_t dataIndex, const std::vector<Vector3<float>>& dataPtr) {
        DataContainer dataContainer(DataType::FLOAT, DataDimension::THREE_DIMENSION, dataPtr.size(), dataPtr.data());
        updateData(dataIndex, std::move(dataContainer));
    }

    void GenericRenderer::updateData(std::size_t dataIndex, DataContainer&& dataContainer) {
        assert(data.size() > dataIndex);
        assert(data[dataIndex].getDataSize() == dataContainer.getDataSize());

        data[dataIndex] = std::move(dataContainer);
    }

    void GenericRenderer::updateShaderData(std::size_t shaderDataIndex, const void* dataPtr) {
        assert(shaderData.size() > shaderDataIndex);

        shaderData[shaderDataIndex].updateData(dataPtr);
    }

    void GenericRenderer::updateTextureReader(std::size_t texturePosition, const std::shared_ptr<TextureReader>& textureReader) {
        updateTextureReaderArray(texturePosition, 0, textureReader);
    }

    const std::shared_ptr<TextureReader>& GenericRenderer::getTextureReader(std::size_t texturePosition) const {
        assert(textureReaders.size() > texturePosition);
        assert(textureReaders[texturePosition].size() == 1);
        return getTextureReaderArray(texturePosition)[0];
    }

    const std::shared_ptr<TextureReader>& GenericRenderer::getTextureReader(std::size_t texturePosition, std::size_t textureIndex) const {
        assert(textureReaders.size() > texturePosition);
        assert(textureReaders[texturePosition].size() > textureIndex);
        return getTextureReaderArray(texturePosition)[textureIndex];
    }

    void GenericRenderer::updateTextureReaderArray(std::size_t texturePosition, std::size_t textureIndex, const std::shared_ptr<TextureReader>& textureReader) {
        assert(textureReaders.size() > texturePosition);
        assert(textureReaders[texturePosition].size() > textureIndex);

        vkDeviceWaitIdle(GraphicService::instance()->getDevices().getLogicalDevice());

        textureReader->initialize();
        textureReaders[texturePosition][textureIndex] = textureReader;

        updateDescriptorSets(); //TODO update only updated ?
    }

    const std::vector<std::shared_ptr<TextureReader>>& GenericRenderer::getTextureReaderArray(std::size_t textureIndex) const {
        assert(textureReaders.size() > textureIndex);

        return textureReaders[textureIndex];
    }

    void GenericRenderer::updateGraphicData(uint32_t frameIndex) {
        //update data (vertex & vertex attributes)
        for (std::size_t dataIndex = 0; dataIndex < data.size(); ++dataIndex) {
            if (data[dataIndex].hasNewData()) {
                drawCommandDirty |= vertexBuffers[dataIndex].updateData(frameIndex, data[dataIndex].getData());
                data[dataIndex].newDataAck(renderTarget->getNumFramebuffer());
            }
        }

        //update shader uniforms
        for (std::size_t shaderDataIndex = 0; shaderDataIndex < shaderData.size(); ++shaderDataIndex) {
            if (shaderData[shaderDataIndex].hasNewData()) {
                drawCommandDirty |= uniformsBuffers[shaderDataIndex].updateData(frameIndex, shaderData[shaderDataIndex].getData());
                shaderData[shaderDataIndex].newDataAck(renderTarget->getNumFramebuffer());
            }
        }
    }

    void GenericRenderer::updateCommandBuffer(VkCommandBuffer commandBuffer, std::size_t frameIndex) {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

        std::array<VkDeviceSize, 20> offsets = {0};
        assert(offsets.size() >= data.size());

        rawVertexBuffers.clear();
        for (const auto& vertexBuffer : vertexBuffers) {
            rawVertexBuffers.emplace_back(vertexBuffer.getBuffer(frameIndex));
        }

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[frameIndex], 0, nullptr);
        vkCmdBindVertexBuffers(commandBuffer, 0, (uint32_t)data.size(), rawVertexBuffers.data(), offsets.data());
        if(indices && indexBuffer.getBuffer(frameIndex)) {
            vkCmdBindIndexBuffer(commandBuffer, indexBuffer.getBuffer(frameIndex), 0, VK_INDEX_TYPE_UINT32);
            vkCmdDrawIndexed(commandBuffer, (uint32_t)indices->getIndicesCount(), 1, 0, 0, 0);
        } else {
            uint32_t vertexCount = 0;
            for (auto& singleData : data) {
                vertexCount += (uint32_t)singleData.getDataCount();
            }

            vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
        }

        drawCommandDirty = false;
    }

}
