#pragma once

#include <graphics/api/vulkan/render/PipelineProcessor.h>
#include <graphics/api/vulkan/render/handler/BufferHandler.h>
#include <graphics/api/vulkan/render/handler/AlterableBufferHandler.h>

namespace urchin {

    class GenericComputeBuilder;
    class TextureReader;

    class GenericCompute final : public PipelineProcessor {
        public:
            explicit GenericCompute(const GenericComputeBuilder&);
            ~GenericCompute() override;

            bool needCommandBufferRefresh(std::size_t) const override;

            bool isDepthTestEnabled() const override;

            void updateUniformData(std::size_t, const void*);

            std::span<OffscreenRender*> getTexturesWriter() const override;

        private:
            void initialize() override;
            void cleanup() override;

            void createPipeline() override;
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
            std::vector<std::shared_ptr<Texture>> uniformTextureOutputs;
            mutable std::vector<OffscreenRender*> texturesWriter;

            std::vector<AlterableBufferHandler> uniformsBuffers;
            VkDescriptorPool descriptorPool;
            std::vector<VkDescriptorSet> descriptorSets;
            std::vector<VkWriteDescriptorSet> descriptorWrites;
            std::vector<VkDescriptorBufferInfo> bufferInfos;
            std::vector<VkDescriptorImageInfo> imageInfosArray;
            std::vector<VkDescriptorImageInfo> imageOutputInfosArray;

            bool drawCommandsDirty;
            std::vector<bool> descriptorSetsDirty;
    };

}
