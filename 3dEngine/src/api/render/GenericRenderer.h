#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include <UrchinCommon.h>

#include <api/render/handler/BufferHandler.h>
#include <api/render/handler/AlterableBufferHandler.h>
#include <api/render/model/ShapeType.h>
#include <api/render/model/DataType.h>
#include <api/render/model/DataDimension.h>
#include <api/render/model/PolygonMode.h>
#include <api/render/model/DataContainer.h>
#include <api/render/model/IndexContainer.h>
#include <api/render/shader/model/Shader.h>
#include <api/render/shader/model/ShaderDataContainer.h>
#include <api/render/blend/BlendFunction.h>
#include <api/render/target/RenderTarget.h>
#include <api/render/pipeline/Pipeline.h>
#include <api/texture/TextureReader.h>
#include <api/render/pipeline/PipelineBuilder.h>

namespace urchin {

    class GenericRendererBuilder;

    class GenericRenderer : public std::enable_shared_from_this<GenericRenderer> {
        public:
            friend class RenderTarget;
            static constexpr uint32_t PRIMITIVE_RESTART_INDEX_VALUE = 0xFFFFFFFF;

            struct RendererComp {
                bool operator()(const GenericRenderer*, const GenericRenderer*) const;
            };

            explicit GenericRenderer(const GenericRendererBuilder*);
            ~GenericRenderer();

            const std::string& getName() const;
            const RenderTarget& getRenderTarget() const;
            bool isDrawCommandDirty() const;

            bool isEnabled() const;
            void enableRenderer(unsigned int);
            void disableRenderer();

            unsigned int getRenderingOrder() const;
            std::size_t getPipelineId() const;

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
            std::shared_ptr<IndexContainer> indices;
            std::vector<ShaderDataContainer> uniformData;
            std::vector<std::vector<std::shared_ptr<TextureReader>>> uniformTextureReaders;
            mutable std::vector<OffscreenRender*> texturesWriter;

            std::unique_ptr<PipelineBuilder> pipelineBuilder;
            std::shared_ptr<Pipeline> pipeline;
            std::vector<AlterableBufferHandler> vertexBuffers;
            AlterableBufferHandler indexBuffer;
            std::vector<AlterableBufferHandler> uniformsBuffers;
            VkDescriptorPool descriptorPool;
            std::vector<VkDescriptorSet> descriptorSets;

            bool drawCommandDirty;
            std::vector<VkBuffer> rawVertexBuffers;
    };

}