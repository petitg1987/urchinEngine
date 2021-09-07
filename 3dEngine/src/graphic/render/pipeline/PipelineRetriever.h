#pragma once

#include <memory>
#include <vulkan/vulkan.h>

#include <graphic/render/pipeline/Pipeline.h>
#include <graphic/render/target/RenderTarget.h>
#include <graphic/render/shader/model/ShaderDataContainer.h>
#include <graphic/render/shader/model/Shader.h>
#include <graphic/render/model/DataContainer.h>
#include <graphic/render/model/ShapeType.h>
#include <graphic/texture/TextureReader.h>

namespace urchin {

    class PipelineRetriever {
        public:
            PipelineRetriever();

            void setupRenderTarget(const RenderTarget&);
            void setupShader(const Shader&);
            void setupShapeType(const ShapeType&);
            void setupData(const std::vector<DataContainer>&);
            void setupUniform(const std::vector<ShaderDataContainer>&, const std::vector<std::vector<std::shared_ptr<TextureReader>>>&);

            std::shared_ptr<Pipeline> getPipeline();

        private:
            std::size_t computePipelineHash() const;

            void createDescriptorSetLayout(const std::shared_ptr<Pipeline>&);
            void createGraphicsPipeline(const std::shared_ptr<Pipeline>&);
            VkPrimitiveTopology shapeTypeToVulkanTopology() const;
            bool isShapeTypeListTopology() const;

            const RenderTarget* renderTarget;
            const Shader* shader;
            const ShapeType* shapeType;
            const std::vector<DataContainer>* data;
            const std::vector<ShaderDataContainer>* uniformData;
            const std::vector<std::vector<std::shared_ptr<TextureReader>>>* uniformTextureReaders;
    };

}
