#ifndef URCHINENGINE_GENERICRENDERER_H
#define URCHINENGINE_GENERICRENDERER_H

#include <vector>
#include <vulkan/vulkan.h>
#include "UrchinCommon.h"

#include "graphic/render/handler/BufferHandler.h"
#include "graphic/render/handler/AlterableBufferHandler.h"
#include "graphic/render/model/ShapeType.h"
#include "graphic/render/model/DataType.h"
#include "graphic/render/model/DataDimension.h"
#include "graphic/render/model/PolygonMode.h"
#include "graphic/render/model/DataContainer.h"
#include "graphic/render/model/IndexContainer.h"
#include "graphic/render/shader/model/Shader.h"
#include "graphic/render/shader/model/ShaderDataContainer.h"
#include "graphic/render/target/RenderTarget.h"
#include "graphic/texture/TextureReader.h"

namespace urchin {

    class GenericRendererBuilder;

    class GenericRenderer : public Observer {
        public:
            static const uint32_t PRIMITIVE_RESTART_INDEX_VALUE;

            explicit GenericRenderer(const GenericRendererBuilder*);
            ~GenericRenderer() override;

            void notify(Observable*, int) override;
            bool isDrawCommandDirty() const;

            void updateData(std::size_t, const std::vector<Point2<float>>&);
            void updateData(std::size_t, const std::vector<Point3<float>>&);
            void updateData(std::size_t, const std::vector<Vector3<float>>&);

            void updateShaderData(std::size_t, const void*);

            void updateTextureReader(std::size_t, const std::shared_ptr<TextureReader>&);
            const std::shared_ptr<TextureReader>& getTextureReader(std::size_t) const;
            void updateTextureReaderArray(std::size_t, const std::vector<std::shared_ptr<TextureReader>>&);
            const std::vector<std::shared_ptr<TextureReader>>& getTextureReaderArray(std::size_t) const;

            void updateGraphicData(uint32_t);
            void updateCommandBuffer(VkCommandBuffer, std::size_t);

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

            void updateData(std::size_t, DataContainer&&);

            static const uint32_t UNIFORM_TEX_BINDING_START_INDEX;

            std::shared_ptr<RenderTarget> renderTarget;
            std::shared_ptr<Shader> shader;
            ShapeType shapeType;

            std::vector<DataContainer> data;
            std::shared_ptr<IndexContainer> indices;
            std::vector<ShaderDataContainer> shaderData;
            std::vector<std::vector<std::shared_ptr<TextureReader>>> textureReaders;
            bool transparencyEnabled;
            bool depthOperationsEnabled;
            bool cullFaceEnabled;
            PolygonMode polygonMode;
            float outlineSize;

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

#endif
