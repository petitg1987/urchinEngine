#include <libs/vma/vk_mem_alloc.h>

#include <graphic/render/GenericRenderer.h>
#include <graphic/helper/DebugLabelHelper.h>
#include <graphic/setup/GraphicService.h>
#include <graphic/render/GenericRendererBuilder.h>
#include <graphic/render/model/DataContainer.h>

namespace urchin {

    GenericRenderer::GenericRenderer(const GenericRendererBuilder* rendererBuilder) :
            isInitialized(false),
            bIsEnabled(true),
            name(rendererBuilder->getName()),
            renderTarget(rendererBuilder->getRenderTarget()),
            shader(rendererBuilder->getShader()),
            shapeType(rendererBuilder->getShapeType()),
            data(rendererBuilder->getData()),
            indices(rendererBuilder->getIndices()),
            uniformData(rendererBuilder->getUniformData()),
            uniformTextureReaders(rendererBuilder->getUniformTextureReaders()),
            blendFunctions(rendererBuilder->getBlendFunctions()),
            depthTestEnabled(rendererBuilder->isDepthTestEnabled()),
            depthWriteEnabled(rendererBuilder->isDepthWriteEnabled()),
            cullFaceEnabled(rendererBuilder->isCullFaceEnabled()),
            polygonMode(rendererBuilder->getPolygonMode()),
            scissorEnabled(rendererBuilder->isScissorEnabled()),
            scissorOffset(rendererBuilder->getScissorOffset()),
            scissorSize(rendererBuilder->getScissorSize()),
            descriptorSetLayout(nullptr),
            pipelineLayout(nullptr),
            graphicsPipeline(nullptr),
            descriptorPool(nullptr),
            drawCommandDirty(false) {
        if ((depthTestEnabled || depthWriteEnabled) && !renderTarget.hasDepthAttachment()) {
            throw std::runtime_error("Depth operations are enabled but there is no depth attachment on the render target");
        }

        for (auto& uniformTextureReaderArray : uniformTextureReaders) {
            for (auto& uniformTextureReader : uniformTextureReaderArray) {
                uniformTextureReader->initialize();
            }
        }

        initialize();
    }

    GenericRenderer::~GenericRenderer() {
        cleanup();
        uniformTextureReaders.clear();
        renderTarget.removeRenderer(this);
    }

    void GenericRenderer::initialize() {
        assert(!isInitialized);

        resetNewDataFlag();
        createDescriptorSetLayout();
        createGraphicsPipeline();
        createVertexBuffers();
        createIndexBuffer();
        createUniformBuffers();
        createDescriptorPool();
        createDescriptorSets();

        isInitialized = true;
    }

    void GenericRenderer::cleanup() {
        if (isInitialized) {
            vkDeviceWaitIdle(GraphicService::instance().getDevices().getLogicalDevice());

            destroyDescriptorSetsAndPool();
            destroyUniformBuffers();
            destroyIndexBuffer();
            destroyVertexBuffers();
            destroyGraphicsPipeline();
            destroyDescriptorSetLayout();

            isInitialized = false;
        }
    }

    const std::string& GenericRenderer::getName() const {
        return name;
    }

    const RenderTarget& GenericRenderer::getRenderTarget() const {
        return renderTarget;
    }

    bool GenericRenderer::isDrawCommandDirty() const {
        return drawCommandDirty;
    }

    bool GenericRenderer::isEnabled() const {
        return bIsEnabled;
    }

    void GenericRenderer::enableRenderer() {
        assert(!bIsEnabled);
        bIsEnabled = true;
        renderTarget.notifyRendererEnabled(this);
    }

    void GenericRenderer::disableRenderer() {
        assert(bIsEnabled);
        bIsEnabled = false;
        renderTarget.notifyRendererDisabled(this);
    }

    void GenericRenderer::createDescriptorSetLayout() {
        ScopeProfiler sp(Profiler::graphic(), "creDescSetLyt");

        auto logicalDevice = GraphicService::instance().getDevices().getLogicalDevice();

        uint32_t shaderUniformBinding = 0;
        std::vector<VkDescriptorSetLayoutBinding> bindings;

        for (auto& uniformSingleData : uniformData) {
            std::ignore = uniformSingleData;
            VkDescriptorSetLayoutBinding uboLayoutBinding{};
            uboLayoutBinding.binding = shaderUniformBinding++;
            uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            uboLayoutBinding.descriptorCount = 1;
            uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
            uboLayoutBinding.pImmutableSamplers = nullptr;
            bindings.emplace_back(uboLayoutBinding);
        }

        for (auto& uniformTextureReader : uniformTextureReaders) {
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

        VkResult result = vkCreateDescriptorSetLayout(logicalDevice, &layoutInfo, nullptr, &descriptorSetLayout);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor set layout with error code: " + std::to_string(result));
        }
    }

    void GenericRenderer::destroyDescriptorSetLayout() {
        vkDestroyDescriptorSetLayout(GraphicService::instance().getDevices().getLogicalDevice(), descriptorSetLayout, nullptr);
    }

    void GenericRenderer::createGraphicsPipeline() {
        ScopeProfiler sp(Profiler::graphic(), "crePipeline");
        auto logicalDevice = GraphicService::instance().getDevices().getLogicalDevice();
        auto shaderStages = shader.getShaderStages();

        //vertex input stage
        std::vector<VkVertexInputBindingDescription> bindingDescriptions;
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
        for (uint32_t i = 0; i < data.size(); ++i) {
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
        inputAssembly.topology = shapeTypeToVulkanTopology();
        inputAssembly.primitiveRestartEnable = isShapeTypeListTopology() ? VK_FALSE : VK_TRUE;

        //viewports and scissors stage
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)renderTarget.getWidth();
        viewport.height = (float)renderTarget.getHeight();
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor = {};
        if (scissorEnabled) {
            scissor.offset = {scissorOffset.X, scissorOffset.Y};
            scissor.extent = {scissorSize.X, scissorSize.Y};
        } else {
            scissor.offset = {0, 0};
            scissor.extent = {renderTarget.getWidth(), renderTarget.getHeight()};
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
        if (!blendFunctions.empty() && blendFunctions.size() != renderTarget.getNumColorAttachment()) {
            throw std::runtime_error("Number of blend functions (" + std::to_string(blendFunctions.size()) + ") does not match with number of color attachments (" + std::to_string(renderTarget.getNumColorAttachment()) + ")");
        } else {
            blendFunctions.resize(renderTarget.getNumColorAttachment(), BlendFunction::buildBlendDisabled());
        }
        std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
        for (std::size_t i = 0; i < renderTarget.getNumColorAttachment(); ++i) {
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
        pipelineInfo.renderPass = renderTarget.getRenderPass();
        pipelineInfo.subpass = 0; //index to sub-pass
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; //can be used to switch optimally between similar pipeline
        pipelineInfo.basePipelineIndex = -1;

        VkResult pipelinesResult = vkCreateGraphicsPipelines(logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline);
        if (pipelinesResult != VK_SUCCESS) {
            throw std::runtime_error("Failed to create graphics pipeline with error code: " + std::to_string(pipelinesResult));
        }

        DebugLabelHelper::nameObject(DebugLabelHelper::PIPELINE, graphicsPipeline, name);
    }

    void GenericRenderer::destroyGraphicsPipeline() {
        auto logicalDevice = GraphicService::instance().getDevices().getLogicalDevice();

        vkDestroyPipeline(logicalDevice, graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(logicalDevice, pipelineLayout, nullptr);
    }

    VkPrimitiveTopology GenericRenderer::shapeTypeToVulkanTopology() const {
        if (shapeType == ShapeType::TRIANGLE) {
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        } else if (shapeType == ShapeType::TRIANGLE_STRIP) {
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        } else if (shapeType == ShapeType::POINT) {
            return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        }
        throw std::runtime_error("Unknown shape type: " + std::to_string(shapeType));
    }

    bool GenericRenderer::isShapeTypeListTopology() const {
        if (shapeType == ShapeType::TRIANGLE || shapeType == ShapeType::POINT) {
            return true;
        } else if (shapeType == ShapeType::TRIANGLE_STRIP) {
            return false;
        }
        throw std::runtime_error("Unknown shape type: " + std::to_string(shapeType));
    }

    void GenericRenderer::createVertexBuffers() {
        ScopeProfiler sp(Profiler::graphic(), "creVertexBuf");

        vertexBuffers.resize(data.size());
        for (std::size_t dataIndex = 0; dataIndex < data.size(); ++dataIndex) {
            const DataContainer& dataContainer = data[dataIndex];
            vertexBuffers[dataIndex].initialize(BufferHandler::VERTEX, renderTarget.getNumFramebuffer(), dataContainer.getBufferSize(), dataContainer.getData());
        }
    }

    void GenericRenderer::destroyVertexBuffers() {
        for (auto& vertexBuffer : vertexBuffers) {
            vertexBuffer.cleanup();
        }
        vertexBuffers.clear();
    }

    void GenericRenderer::createIndexBuffer() {
        ScopeProfiler sp(Profiler::graphic(), "creIndexBuf");

        if (indices) {
            indexBuffer.initialize(BufferHandler::INDEX, renderTarget.getNumFramebuffer(), indices->getBufferSize(), indices->getIndices());
        }
    }

    void GenericRenderer::destroyIndexBuffer() {
        if (indices) {
            indexBuffer.cleanup();
        }
    }

    void GenericRenderer::createUniformBuffers() {
        ScopeProfiler sp(Profiler::graphic(), "creUniformBuf");

        uniformsBuffers.resize(uniformData.size());
        for (std::size_t dataIndex = 0; dataIndex < uniformData.size(); ++dataIndex) {
            uniformsBuffers[dataIndex].initialize(BufferHandler::UNIFORM, renderTarget.getNumFramebuffer(), uniformData[dataIndex].getDataSize());
        }
    }

    void GenericRenderer::destroyUniformBuffers() {
        for (auto& uniformsBuffer : uniformsBuffers) {
            uniformsBuffer.cleanup();
        }
        uniformsBuffers.clear();
    }

    void GenericRenderer::createDescriptorPool() {
        ScopeProfiler sp(Profiler::graphic(), "creDescPool");
        auto logicalDevice = GraphicService::instance().getDevices().getLogicalDevice();

        std::array<VkDescriptorPoolSize, 2> poolSizes{};

        int uboDescriptorCount = std::max(1, (int)renderTarget.getNumFramebuffer() * (int)uniformData.size());
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = (uint32_t)uboDescriptorCount;

        int uniformTexReadersCount = 0;
        std::for_each(uniformTextureReaders.begin(), uniformTextureReaders.end(), [&] (auto& r){uniformTexReadersCount += (int)r.size();});
        int textureDescriptorCount = std::max(1, (int)renderTarget.getNumFramebuffer() * uniformTexReadersCount);
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = (uint32_t)textureDescriptorCount;

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = (uint32_t)poolSizes.size();
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = (uint32_t)renderTarget.getNumFramebuffer();

        VkResult result = vkCreateDescriptorPool(logicalDevice, &poolInfo, nullptr, &descriptorPool);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor pool with error code: " + std::to_string(result));
        }
    }

    void GenericRenderer::createDescriptorSets() {
        ScopeProfiler sp(Profiler::graphic(), "creDescSet");
        auto logicalDevice = GraphicService::instance().getDevices().getLogicalDevice();

        std::vector<VkDescriptorSetLayout> layouts(renderTarget.getNumFramebuffer(), descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = (uint32_t)renderTarget.getNumFramebuffer();
        allocInfo.pSetLayouts = layouts.data();

        descriptorSets.resize(renderTarget.getNumFramebuffer());
        VkResult result = vkAllocateDescriptorSets(logicalDevice, &allocInfo, descriptorSets.data());
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate descriptor sets with error code: " + std::to_string(result));
        }

        updateDescriptorSets();
    }

    void GenericRenderer::updateDescriptorSets() {
        ScopeProfiler sp(Profiler::graphic(), "updateDescSet");
        auto logicalDevice = GraphicService::instance().getDevices().getLogicalDevice();

        for (std::size_t frameIndex = 0; frameIndex < renderTarget.getNumFramebuffer(); frameIndex++) {
            std::vector<VkWriteDescriptorSet> descriptorWrites;
            uint32_t shaderUniformBinding = 0;

            //uniform buffer objects
            std::vector<VkDescriptorBufferInfo> bufferInfos;
            for (std::size_t uniformDataIndex = 0; uniformDataIndex < uniformData.size(); ++uniformDataIndex) {
                VkDescriptorBufferInfo bufferInfo{};
                bufferInfo.buffer = uniformsBuffers[uniformDataIndex].getBuffer(frameIndex);
                bufferInfo.offset = 0;
                bufferInfo.range = VK_WHOLE_SIZE;
                bufferInfos.emplace_back(bufferInfo);
            }
            for (auto& bufferInfo : bufferInfos) {
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
            std::vector<std::vector<VkDescriptorImageInfo>> imageInfos;
            for (auto& uniformTextureReaderArray : uniformTextureReaders) {
                std::vector<VkDescriptorImageInfo> imageInfosArray;
                for (auto& uniformTextureReader : uniformTextureReaderArray) {
                    VkDescriptorImageInfo imageInfo{};
                    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    imageInfo.imageView = uniformTextureReader->getTexture()->getImageView();
                    imageInfo.sampler = uniformTextureReader->getParam().getTextureSampler();
                    imageInfosArray.emplace_back(imageInfo);
                }
                imageInfos.emplace_back(imageInfosArray);
            }
            for (auto& imageInfo : imageInfos) {
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
        vkDestroyDescriptorPool(GraphicService::instance().getDevices().getLogicalDevice(), descriptorPool, nullptr);
    }

    void GenericRenderer::resetNewDataFlag() {
        for (auto& singleData : data) {
            singleData.resetNewDataFlag();
        }

        for (auto& singleUniformData : uniformData) {
            singleUniformData.resetNewDataFlag();
        }
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
        data[dataIndex] = std::move(dataContainer);
    }

    void GenericRenderer::updateUniformData(std::size_t uniformDataIndex, const void* dataPtr) {
        assert(uniformData.size() > uniformDataIndex);
        uniformData[uniformDataIndex].updateData(dataPtr);
    }

    void GenericRenderer::updateUniformTextureReader(std::size_t uniformTexPosition, const std::shared_ptr<TextureReader>& textureReader) {
        updateUniformTextureReaderArray(uniformTexPosition, 0, textureReader);
    }

    const std::shared_ptr<TextureReader>& GenericRenderer::getUniformTextureReader(std::size_t uniformTexPosition) const {
        assert(uniformTextureReaders.size() > uniformTexPosition);
        assert(uniformTextureReaders[uniformTexPosition].size() == 1);
        return getUniformTextureReaderArray(uniformTexPosition)[0];
    }

    const std::shared_ptr<TextureReader>& GenericRenderer::getUniformTextureReader(std::size_t uniformTexPosition, std::size_t textureIndex) const {
        assert(uniformTextureReaders.size() > uniformTexPosition);
        assert(uniformTextureReaders[uniformTexPosition].size() > textureIndex);
        return getUniformTextureReaderArray(uniformTexPosition)[textureIndex];
    }

    void GenericRenderer::updateUniformTextureReaderArray(std::size_t uniformTexPosition, std::size_t textureIndex, const std::shared_ptr<TextureReader>& textureReader) {
        assert(uniformTextureReaders.size() > uniformTexPosition);
        assert(uniformTextureReaders[uniformTexPosition].size() > textureIndex);

        vkDeviceWaitIdle(GraphicService::instance().getDevices().getLogicalDevice());

        textureReader->initialize();
        uniformTextureReaders[uniformTexPosition][textureIndex] = textureReader;

        updateDescriptorSets();
    }

    const std::vector<std::shared_ptr<TextureReader>>& GenericRenderer::getUniformTextureReaderArray(std::size_t textureIndex) const {
        assert(uniformTextureReaders.size() > textureIndex);
        return uniformTextureReaders[textureIndex];
    }

    const std::vector<OffscreenRender*>& GenericRenderer::getTexturesWriter() const {
        texturesWriter.clear();

        for (auto& uniformTextureReaderArray : uniformTextureReaders) {
            for (auto& uniformTextureReader : uniformTextureReaderArray) {
                auto lastTextureWriter = uniformTextureReader->getTexture()->getLastTextureWriter();
                if (lastTextureWriter) {
                    texturesWriter.emplace_back(lastTextureWriter);
                }
            }
        }

        return texturesWriter;
    }

    void GenericRenderer::updateGraphicData(uint32_t frameIndex) {
        //update data (vertex & vertex attributes)
        #ifndef NDEBUG
            std::size_t dataCount = data[0].getDataCount();
            for (std::size_t dataIndex = 1; dataIndex < data.size(); ++dataIndex) {
                assert(indices || dataCount == data[dataIndex].getDataCount());
            }
        #endif
        for (std::size_t dataIndex = 0; dataIndex < data.size(); ++dataIndex) {
            if (data[dataIndex].hasNewData(frameIndex)) {
                auto& dataContainer = data[dataIndex];
                drawCommandDirty |= vertexBuffers[dataIndex].updateData(frameIndex, dataContainer.getBufferSize(), dataContainer.getData());
                data[dataIndex].newDataAck(frameIndex);
            }
        }

        //update shader uniforms
        for (std::size_t uniformDataIndex = 0; uniformDataIndex < uniformData.size(); ++uniformDataIndex) {
            if (uniformData[uniformDataIndex].hasNewData(frameIndex)) {
                auto& dataContainer = uniformData[uniformDataIndex];
                drawCommandDirty |= uniformsBuffers[uniformDataIndex].updateData(frameIndex, dataContainer.getDataSize(), dataContainer.getData());
                uniformData[uniformDataIndex].newDataAck(frameIndex);
            }
        }
    }

    void GenericRenderer::updateCommandBuffer(VkCommandBuffer commandBuffer, std::size_t frameIndex) {
        ScopeProfiler sp(Profiler::graphic(), "updateCmdBuf");
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

        std::array<VkDeviceSize, 20> offsets = {0};
        assert(offsets.size() >= data.size());

        rawVertexBuffers.clear();
        for (const auto& vertexBuffer : vertexBuffers) {
            rawVertexBuffers.emplace_back(vertexBuffer.getBuffer(frameIndex));
        }

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[frameIndex], 0, nullptr);
        vkCmdBindVertexBuffers(commandBuffer, 0, (uint32_t)data.size(), rawVertexBuffers.data(), offsets.data());
        if (indices && indexBuffer.getBuffer(frameIndex)) {
            vkCmdBindIndexBuffer(commandBuffer, indexBuffer.getBuffer(frameIndex), 0, VK_INDEX_TYPE_UINT32);
            vkCmdDrawIndexed(commandBuffer, (uint32_t)indices->getIndicesCount(), 1, 0, 0, 0);
        } else {
            auto vertexCount = (uint32_t)data[0].getDataCount();
            vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
        }

        drawCommandDirty = false;
    }

}
