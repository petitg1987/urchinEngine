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

            bool isDepthTestEnabled() const override;

            void updateData(std::size_t, const std::vector<Point2<float>>&);
            void updateData(std::size_t, const std::vector<Point3<float>>&);
            void updateData(std::size_t, const std::vector<Vector3<float>>&);
            void updateInstanceData(std::size_t, const float*) const;

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

            void updatePipelineProcessorData(uint32_t) override;
            void doUpdateCommandBuffer(VkCommandBuffer, std::size_t, std::size_t) override;

            bool isInitialized;

            std::vector<DataContainer> data;
            std::shared_ptr<DataContainer> instanceData;
            std::shared_ptr<IndexContainer> indices;
            bool customScissor;
            Vector2<int> scissorOffset;
            Vector2<int> scissorSize;

            bool depthTestEnabled;
            std::vector<AlterableBufferHandler> vertexBuffers;
            AlterableBufferHandler instanceVertexBuffer;
            AlterableBufferHandler indexBuffer;

            std::vector<VkBuffer> rawVertexBuffers;
    };

}
