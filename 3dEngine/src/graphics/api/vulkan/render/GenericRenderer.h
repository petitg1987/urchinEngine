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
#include <graphics/api/vulkan/render/PipelineProcessor.h>

namespace urchin {

    class GenericRendererBuilder;
    class TextureReader;

    class GenericRenderer final : public PipelineProcessor {
        public:
            static constexpr uint32_t PRIMITIVE_RESTART_INDEX_VALUE = 0xFFFFFFFF;

            explicit GenericRenderer(const GenericRendererBuilder&);
            ~GenericRenderer() override;

            bool needCommandBufferRefresh(std::size_t) const override;

            bool isDepthTestEnabled() const override;

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
            std::span<OffscreenRender*> getTexturesWriter() const override;

            void resetScissor();
            void updateScissor(Vector2<int>, Vector2<int>);

        private:
            void initialize() override;
            void cleanup() override;

            void createPipeline() override;
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

            void updateGraphicData(uint32_t) override;
            std::size_t updateCommandBuffer(VkCommandBuffer, std::size_t, std::size_t) override;

            bool isInitialized;

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
