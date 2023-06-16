#pragma once

#include <graphics/api/vulkan/render/target/RenderTarget.h>
#include <graphics/api/vulkan/render/shader/Shader.h>
#include <graphics/api/vulkan/render/pipeline/Pipeline.h>
#include <graphics/api/vulkan/render/pipeline/PipelineBuilder.h>
#include <graphics/api/vulkan/render/handler/BufferHandler.h>
#include <graphics/api/vulkan/render/handler/AlterableBufferHandler.h>

namespace urchin {

    class PipelineProcessor { //TODO re-sort methods
        public:
            friend class RenderTarget;

            PipelineProcessor(std::string, RenderTarget&, const Shader&,
                              const std::vector<ShaderDataContainer>&,
                              const std::vector<std::vector<std::shared_ptr<TextureReader>>>&,
                              const std::vector<std::shared_ptr<Texture>>&);
            virtual ~PipelineProcessor();

            const std::string& getName() const;
            const RenderTarget& getRenderTarget() const;
            const Shader& getShader() const;
            bool needCommandBufferRefresh(std::size_t) const;

            bool isEnabled() const;
            void enableRenderer(unsigned int);
            void disableRenderer();
            unsigned int getRenderingOrder() const;

            virtual bool isDepthTestEnabled() const = 0;
            std::size_t getPipelineId() const;
            PipelineType getPipelineType() const;

            void updateUniformData(std::size_t, const void*);

            void updateUniformTextureReader(std::size_t, const std::shared_ptr<TextureReader>&);
            const std::shared_ptr<TextureReader>& getUniformTextureReader(std::size_t) const;
            const std::shared_ptr<TextureReader>& getUniformTextureReader(std::size_t, std::size_t) const;
            void updateUniformTextureReaderArray(std::size_t, std::size_t, const std::shared_ptr<TextureReader>&);
            const std::vector<std::shared_ptr<TextureReader>>& getUniformTextureReaderArray(std::size_t) const;
            std::span<OffscreenRender*> getTexturesWriter() const;

        protected:
            void setupPipelineBuilder(std::unique_ptr<PipelineBuilder>);
            PipelineBuilder& getPipelineBuilder() const;
            virtual void createPipeline();
            void destroyPipeline();
            Pipeline& getPipeline() const;

            void createUniformBuffers();
            void destroyUniformBuffers();
            void createDescriptorPool();
            void createDescriptorSets();
            void updateDescriptorSets();
            void updateDescriptorSets(std::size_t);
            void destroyDescriptorSetsAndPool();

            void markDrawCommandsDirty();
            void updateShaderUniforms(uint32_t);
            std::vector<VkDescriptorSet>& getDescriptorSets();

        private:
            virtual void initialize() = 0;
            virtual void cleanup() = 0;

            virtual void updateGraphicData(uint32_t) = 0;
            std::size_t updateCommandBuffer(VkCommandBuffer, std::size_t, std::size_t);
            virtual void doUpdateCommandBuffer(VkCommandBuffer, std::size_t, std::size_t) = 0;

            std::string name;
            RenderTarget& renderTarget;
            const Shader& shader;

            bool bIsEnabled;
            unsigned int renderingOrder;
            std::vector<ShaderDataContainer> uniformData;
            std::vector<std::vector<std::shared_ptr<TextureReader>>> uniformTextureReaders;
            std::vector<std::shared_ptr<Texture>> uniformTextureOutputs;
            mutable std::vector<OffscreenRender*> texturesWriter;

            std::unique_ptr<PipelineBuilder> pipelineBuilder;
            std::shared_ptr<Pipeline> pipeline;

            VkDescriptorPool descriptorPool;
            std::vector<VkDescriptorSet> descriptorSets;
            std::vector<AlterableBufferHandler> uniformsBuffers;
            std::vector<VkWriteDescriptorSet> descriptorWrites;
            std::vector<VkDescriptorBufferInfo> bufferInfos;
            std::vector<VkDescriptorImageInfo> imageInfosArray;
            std::vector<VkDescriptorImageInfo> imageOutputInfosArray;

            std::vector<bool> descriptorSetsDirty;
            bool drawCommandsDirty;
    };

}
