#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include <UrchinCommon.h>

#include <graphics/render/shader/ShaderDataContainer.h>
#include <graphics/render/data/ShapeType.h>
#include <graphics/render/data/DataType.h>
#include <graphics/render/data/VariableType.h>
#include <graphics/render/data/PolygonMode.h>
#include <graphics/render/data/DataContainer.h>
#include <graphics/render/data/IndexContainer.h>
#include <graphics/render/data/BlendFunction.h>
#include <graphics/api/vulkan/render/handler/BufferHandler.h>
#include <graphics/api/vulkan/render/handler/AlterableBufferHandler.h>
#include <graphics/api/vulkan/render/shader/Shader.h>
#include <graphics/api/vulkan/render/target/RenderTarget.h>
#include <graphics/api/vulkan/render/pipeline/Pipeline.h>
#include <graphics/api/vulkan/render/pipeline/PipelineBuilder.h>

namespace urchin {

    class GenericRendererBuilder;
    class TextureReader;

    class GenericRenderer {
        public:
            friend class RenderTarget;
            static constexpr uint32_t PRIMITIVE_RESTART_INDEX_VALUE = 0xFFFFFFFF;

            explicit GenericRenderer(const GenericRendererBuilder&);
            ~GenericRenderer();

            const std::string& getName() const;
            const RenderTarget& getRenderTarget() const;
            bool needCommandBufferRefresh(std::size_t) const;

            bool isEnabled() const;
            void enableRenderer(unsigned int);
            void disableRenderer();

            unsigned int getRenderingOrder() const;
            bool isDepthTestEnabled() const;
            std::size_t getPipelineId() const;

            void updateData(std::size_t, const std::vector<Point2<float>>&);
            void updateData(std::size_t, const std::vector<Point3<float>>&);
            void updateData(std::size_t, const std::vector<Vector3<float>>&);
            void updateInstanceData(std::size_t, const float*) const;

            void updateUniformData(std::size_t, const void*);

            void updateUniformTextureReader(std::size_t, const std::shared_ptr<TextureReader>&);
            const std::shared_ptr<TextureReader>& getUniformTextureReader(std::size_t) const;
            const std::shared_ptr<TextureReader>& getUniformTextureReader(std::size_t, std::size_t) const;
            void updateUniformTextureReaderArray(std::size_t, std::size_t, const std::shared_ptr<TextureReader>&);
            const std::vector<std::shared_ptr<TextureReader>>& getUniformTextureReaderArray(std::size_t) const;
            std::span<OffscreenRender*> getTexturesWriter() const;

            void resetScissor();
            void updateScissor(Vector2<int>, Vector2<int>);

        private:
            void initialize();
            void cleanup();

            void createPipeline();
            void destroyPipeline();
            void createVertexBuffers();
            void destroyVertexBuffers();
            void createIndexBuffer();
            void destroyIndexBuffer();
            void createUniformBuffers();
            void destroyUniformBuffers();
            void createDescriptorPool();
            void createDescriptorSets();
            void updateDescriptorSets();
            void updateDescriptorSets(std::size_t);
            void destroyDescriptorSetsAndPool();

            void updateGraphicData(uint32_t);
            std::size_t updateCommandBuffer(VkCommandBuffer, std::size_t, std::size_t);

            bool isInitialized;
            bool bIsEnabled;
            unsigned int renderingOrder;

            std::string name;
            RenderTarget& renderTarget;
            const Shader& shader;

            std::vector<DataContainer> data;
            std::shared_ptr<DataContainer> instanceData;
            std::shared_ptr<IndexContainer> indices;
            std::vector<ShaderDataContainer> uniformData;
            std::vector<std::vector<std::shared_ptr<TextureReader>>> uniformTextureReaders;
            mutable std::vector<OffscreenRender*> texturesWriter;
            bool customScissor;
            Vector2<int> scissorOffset;
            Vector2<int> scissorSize;

            bool depthTestEnabled;
            std::unique_ptr<PipelineBuilder> pipelineBuilder;
            std::shared_ptr<Pipeline> pipeline;
            std::vector<AlterableBufferHandler> vertexBuffers;
            AlterableBufferHandler instanceVertexBuffer;
            AlterableBufferHandler indexBuffer;
            std::vector<AlterableBufferHandler> uniformsBuffers;
            VkDescriptorPool descriptorPool;
            std::vector<VkDescriptorSet> descriptorSets;
            std::vector<VkWriteDescriptorSet> descriptorWrites;
            std::vector<VkDescriptorBufferInfo> bufferInfos;
            std::vector<VkDescriptorImageInfo> imageInfosArray;

            bool drawCommandsDirty;
            std::vector<bool> descriptorSetsDirty;
            std::vector<VkBuffer> rawVertexBuffers;
    };

}
