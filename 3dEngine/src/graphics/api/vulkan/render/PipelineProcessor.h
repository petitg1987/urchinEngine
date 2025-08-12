#pragma once

#include <map>

#include "graphics/api/vulkan/render/target/RenderTarget.h"
#include "graphics/api/vulkan/render/shader/Shader.h"
#include "graphics/api/vulkan/render/pipeline/Pipeline.h"
#include "graphics/api/vulkan/render/pipeline/PipelineBuilder.h"
#include "graphics/api/vulkan/render/handler/BufferHandler.h"
#include "graphics/api/vulkan/render/handler/AlterableBufferHandler.h"

namespace urchin {

    class PipelineProcessor {
        public:
            PipelineProcessor(std::string, RenderTarget&, const Shader&,
                              const std::map<uint32_t, ShaderDataContainer>&,
                              const std::map<uint32_t, std::vector<std::shared_ptr<TextureReader>>>&,
                              const std::map<uint32_t, std::shared_ptr<Texture>>&,
                              const std::map<uint32_t, ShaderDataContainer>&);
            virtual ~PipelineProcessor();

            const std::string& getName() const;
            const RenderTarget& getRenderTarget() const;
            const Shader& getShader() const;

            virtual void initialize() = 0;
            virtual void cleanup() = 0;

            bool isEnabled() const;
            void enableRenderer(unsigned int);
            void disableRenderer();
            unsigned int getRenderingOrder() const;
            virtual bool isGraphicsProcessor() const = 0;
            virtual bool isComputeProcessor() const = 0;
            virtual bool isDepthTestEnabled() const = 0;
            std::size_t getPipelineId() const;
            PipelineType getPipelineType() const;

            void updateUniformData(uint32_t, const void*);
            void updateUniformTextureReader(uint32_t, const std::shared_ptr<TextureReader>&);
            const std::shared_ptr<TextureReader>& getUniformTextureReader(uint32_t) const;
            const std::shared_ptr<TextureReader>& getUniformTextureReader(uint32_t, std::size_t) const;
            void updateUniformTextureReaderArray(uint32_t, std::size_t, const std::shared_ptr<TextureReader>&);
            const std::vector<std::shared_ptr<TextureReader>>& getUniformTextureReaderArray(uint32_t) const;
            void updateStorageBufferData(uint32_t, const void*);
            std::span<OffscreenRender*> getTexturesWriter() const;

            virtual void updatePipelineProcessorData(uint32_t);
            virtual bool needCommandBufferRefresh(std::size_t) const;
            std::size_t updateCommandBuffer(VkCommandBuffer, std::size_t, std::size_t, std::size_t);

        protected:
            void setupPipelineBuilder(std::unique_ptr<PipelineBuilder>);
            PipelineBuilder& getPipelineBuilder() const;
            virtual void createPipeline();
            void destroyPipeline();
            Pipeline& getPipeline() const;

            void createUniformBuffers();
            void destroyUniformBuffers();
            void createStorageBuffers();
            void destroyStorageBuffers();
            void createDescriptorPool();
            void createDescriptorSets();
            void updateDescriptorSets();
            void updateDescriptorSets(std::size_t);
            void destroyDescriptorSetsAndPool() const;

            void markDrawCommandsDirty();
            std::vector<VkDescriptorSet>& getDescriptorSets();
            virtual bool isApplicableOnLayer(std::size_t) const = 0;
            virtual std::size_t doUpdateCommandBuffer(VkCommandBuffer, std::size_t, std::size_t, std::size_t) = 0;

        private:
            std::string name;
            RenderTarget& renderTarget;
            const Shader& shader;

            bool bIsEnabled;
            unsigned int renderingOrder;
            std::unique_ptr<PipelineBuilder> pipelineBuilder;
            std::shared_ptr<Pipeline> pipeline;

            std::map<uint32_t, ShaderDataContainer> uniformData;
            std::map<uint32_t, std::vector<std::shared_ptr<TextureReader>>> uniformTextureReaders;
            std::map<uint32_t, std::shared_ptr<Texture>> uniformTextureOutputs;
            std::map<uint32_t, ShaderDataContainer> storageBufferData;
            mutable std::vector<OffscreenRender*> texturesWriter;

            VkDescriptorPool descriptorPool;
            std::vector<VkDescriptorSet> descriptorSets;
            std::map<uint32_t, AlterableBufferHandler> uniformsBuffers;
            std::map<uint32_t, AlterableBufferHandler> storageBuffers;
            std::vector<VkWriteDescriptorSet> descriptorWrites;
            std::vector<VkDescriptorBufferInfo> uniformBufferInfos;
            std::vector<VkDescriptorImageInfo> imageInfosArray;
            std::vector<VkDescriptorImageInfo> imageOutputInfosArray;
            std::vector<VkDescriptorBufferInfo> storageBufferInfos;

            std::vector<bool> descriptorSetsDirty;
            bool drawCommandsDirty;
    };

}
