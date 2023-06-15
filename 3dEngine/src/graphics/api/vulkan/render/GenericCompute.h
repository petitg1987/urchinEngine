#pragma once

#include <graphics/api/vulkan/render/PipelineProcessor.h>
#include <graphics/api/vulkan/render/handler/BufferHandler.h>
#include <graphics/api/vulkan/render/handler/AlterableBufferHandler.h>
#include <graphics/api/vulkan/render/pipeline/PipelineBuilder.h>

namespace urchin {

    class GenericComputeBuilder;
    class TextureReader;

    class GenericCompute final : public PipelineProcessor {
        public:
            explicit GenericCompute(const GenericComputeBuilder&);
            ~GenericCompute() override;

            bool needCommandBufferRefresh(std::size_t) const override;

            bool isDepthTestEnabled() const override;
            std::size_t getPipelineId() const override;
            PipelineType getPipelineType() const override;

            void updateUniformData(std::size_t, const void*);

            std::span<OffscreenRender*> getTexturesWriter() const override;

        private:
            void initialize() override;
            void cleanup() override;

            void createPipeline();
            void destroyPipeline();
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

            std::vector<ShaderDataContainer> uniformData;
            std::vector<std::vector<std::shared_ptr<TextureReader>>> uniformTextureReaders;
            mutable std::vector<OffscreenRender*> texturesWriter;

            std::unique_ptr<PipelineBuilder> pipelineBuilder;
            std::shared_ptr<Pipeline> pipeline;
            std::vector<AlterableBufferHandler> uniformsBuffers;
            VkDescriptorPool descriptorPool;
            std::vector<VkDescriptorSet> descriptorSets;
            std::vector<VkWriteDescriptorSet> descriptorWrites;
            std::vector<VkDescriptorBufferInfo> bufferInfos;
            std::vector<VkDescriptorImageInfo> imageInfosArray;

            bool drawCommandsDirty;
            std::vector<bool> descriptorSetsDirty;
    };

}
