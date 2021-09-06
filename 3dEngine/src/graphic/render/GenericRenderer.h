#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include <UrchinCommon.h>

#include <graphic/render/handler/BufferHandler.h>
#include <graphic/render/handler/AlterableBufferHandler.h>
#include <graphic/render/model/ShapeType.h>
#include <graphic/render/model/DataType.h>
#include <graphic/render/model/DataDimension.h>
#include <graphic/render/model/PolygonMode.h>
#include <graphic/render/model/DataContainer.h>
#include <graphic/render/model/IndexContainer.h>
#include <graphic/render/shader/model/Shader.h>
#include <graphic/render/shader/model/ShaderDataContainer.h>
#include <graphic/render/blend/BlendFunction.h>
#include <graphic/render/target/RenderTarget.h>
#include <graphic/texture/TextureReader.h>

namespace urchin {

    class GenericRendererBuilder;

    class GenericRenderer : public std::enable_shared_from_this<GenericRenderer> {
        public:
            friend class RenderTarget;
            static constexpr uint32_t PRIMITIVE_RESTART_INDEX_VALUE = 0xFFFFFFFF;

            explicit GenericRenderer(const GenericRendererBuilder*);
            ~GenericRenderer();

            const std::string& getName() const;
            const RenderTarget& getRenderTarget() const;
            bool isDrawCommandDirty() const;

            bool isEnabled() const;
            void enableRenderer();
            void disableRenderer();

            void updateData(std::size_t, const std::vector<Point2<float>>&);
            void updateData(std::size_t, const std::vector<Point3<float>>&);
            void updateData(std::size_t, const std::vector<Vector3<float>>&);

            void updateUniformData(std::size_t, const void*);

            void updateUniformTextureReader(std::size_t, const std::shared_ptr<TextureReader>&);
            const std::shared_ptr<TextureReader>& getUniformTextureReader(std::size_t) const;
            const std::shared_ptr<TextureReader>& getUniformTextureReader(std::size_t, std::size_t) const;
            void updateUniformTextureReaderArray(std::size_t, std::size_t, const std::shared_ptr<TextureReader>&);
            const std::vector<std::shared_ptr<TextureReader>>& getUniformTextureReaderArray(std::size_t) const;
            const std::vector<OffscreenRender*>& getTexturesWriter() const;

        private:
            void initialize();
            void cleanup();

            void createDescriptorSetLayout();
            void destroyDescriptorSetLayout();
            void createGraphicsPipeline();
            VkPrimitiveTopology shapeTypeToVulkanType() const;
            void destroyGraphicsPipeline();
            void createVertexBuffers();
            void destroyVertexBuffers();
            void createIndexBuffer();
            void destroyIndexBuffer();
            void createUniformBuffers();
            void destroyUniformBuffers();
            void createDescriptorPool();
            void createDescriptorSets();
            void updateDescriptorSets();
            void destroyDescriptorSetsAndPool();
            void resetNewDataFlag();

            void updateData(std::size_t, DataContainer&&);

            void updateGraphicData(uint32_t);
            void updateCommandBuffer(VkCommandBuffer, std::size_t);

            bool isInitialized;
            bool bIsEnabled;

            std::string name;
            RenderTarget& renderTarget;
            const Shader& shader;
            ShapeType shapeType;

            std::vector<DataContainer> data;
            std::shared_ptr<IndexContainer> indices;
            std::vector<ShaderDataContainer> uniformData;
            std::vector<std::vector<std::shared_ptr<TextureReader>>> uniformTextureReaders;
            mutable std::vector<OffscreenRender*> texturesWriter;
            std::vector<BlendFunction> blendFunctions;
            bool depthTestEnabled;
            bool depthWriteEnabled;
            bool cullFaceEnabled;
            PolygonMode polygonMode;
            bool scissorEnabled;
            Vector2<int> scissorOffset;
            Vector2<unsigned int> scissorSize;

            VkDescriptorSetLayout descriptorSetLayout;
            VkPipelineLayout pipelineLayout;
            VkPipeline graphicsPipeline;
            std::vector<AlterableBufferHandler> vertexBuffers;
            AlterableBufferHandler indexBuffer;
            std::vector<AlterableBufferHandler> uniformsBuffers;
            VkDescriptorPool descriptorPool;
            std::vector<VkDescriptorSet> descriptorSets;

            bool drawCommandDirty;
            std::vector<VkBuffer> rawVertexBuffers;
    };

}
