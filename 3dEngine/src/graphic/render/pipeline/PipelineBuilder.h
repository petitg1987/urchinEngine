#pragma once

#include <memory>
#include <vulkan/vulkan.h>

#include <graphic/render/pipeline/Pipeline.h>
#include <graphic/render/target/RenderTarget.h>
#include <graphic/render/shader/model/ShaderDataContainer.h>
#include <graphic/render/shader/model/Shader.h>
#include <graphic/render/model/DataContainer.h>
#include <graphic/render/model/ShapeType.h>
#include <graphic/render/blend/BlendFunction.h>
#include <graphic/render/model/PolygonMode.h>
#include <graphic/texture/TextureReader.h>

namespace urchin {

    class PipelineBuilder {
        public:
            explicit PipelineBuilder(std::string);

            void setupRenderTarget(const RenderTarget&);
            void setupShader(const Shader&);
            void setupShapeType(const ShapeType&);
            void setupBlendFunctions(const std::vector<BlendFunction>&);
            void setupDepthOperations(bool, bool);
            void setupCallFaceOperation(bool);
            void setupPolygonMode(PolygonMode);
            void setupScissor(bool, Vector2<int>, Vector2<unsigned int>);
            void setupData(const std::vector<DataContainer>&);
            void setupUniform(const std::vector<ShaderDataContainer>&, const std::vector<std::vector<std::shared_ptr<TextureReader>>>&);

            std::shared_ptr<Pipeline> buildPipeline();

        private:
            void checkSetup();
            std::size_t computePipelineHash() const;

            void createDescriptorSetLayout(const std::shared_ptr<Pipeline>&);
            void createGraphicsPipeline(const std::shared_ptr<Pipeline>&);
            VkPrimitiveTopology shapeTypeToVulkanTopology() const;
            bool isShapeTypeListTopology() const;

            std::string name;
            const RenderTarget* renderTarget;
            const Shader* shader;
            ShapeType shapeType;

            const std::vector<DataContainer>* data;
            const std::vector<ShaderDataContainer>* uniformData;
            const std::vector<std::vector<std::shared_ptr<TextureReader>>>* uniformTextureReaders;

            std::vector<BlendFunction> blendFunctions;
            bool depthTestEnabled;
            bool depthWriteEnabled;
            bool cullFaceEnabled;
            PolygonMode polygonMode;
            bool scissorEnabled;
            Vector2<int> scissorOffset;
            Vector2<unsigned int> scissorSize;
    };

}
